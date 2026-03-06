#include "file.h"
#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_STATUS read_file(EFI_HANDLE image, CHAR16 *kernelConfigFileName)
{
	EFI_FILE_HANDLE fileHandle;
	Print(L"Searching for file...\n");
	EFI_FILE_HANDLE rootVolume = open_volume(image);

	EFI_STATUS status =
		uefi_call_wrapper(rootVolume->Open, 5, rootVolume, &fileHandle, kernelConfigFileName, EFI_FILE_READ_ONLY, 0);

	if (EFI_ERROR(status))
	{
		Print(L"File could not be opened");
	}
	else
	{
		Print(L"File opened...\n");
	}

	UINT64 readSize = file_size(fileHandle);
	UINT8 *buffer = AllocatePool((UINTN)readSize);

	status = uefi_call_wrapper(fileHandle->Read, 3, fileHandle, &readSize, buffer);

	if (EFI_ERROR(status))
	{
		Print(L"Error reading file: %p\n", status);
	}
	else
	{
		Print(L"File read successfully\n");
	}

	// TODO: Remove and read into kernel
	for (UINT64 i = 0; i <= readSize; i++)
	{
		if (i == readSize)
		{
			CHAR8 newLine = L'\n';
			buffer[i] = newLine;
		}
		Print(L"%c", buffer[i]);
	}
	status = uefi_call_wrapper(fileHandle->Close, 1, fileHandle);

	return status;
}

EFI_FILE_HANDLE open_volume(EFI_HANDLE handle)
{
	EFI_LOADED_IMAGE	  *loadedImage = NULL;
	EFI_FILE_IO_INTERFACE *fileIoVolume;
	EFI_FILE_HANDLE		   volume;
	EFI_GUID			   loadImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_GUID			   fileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

	uefi_call_wrapper(BS->HandleProtocol, 3, handle, &loadImageGuid, (VOID **)&loadedImage);

	uefi_call_wrapper(BS->HandleProtocol, 3, loadedImage->DeviceHandle, &fileSystemGuid, (VOID *)&fileIoVolume);
	uefi_call_wrapper(fileIoVolume->OpenVolume, 2, fileIoVolume, &volume);
	return volume;
}

UINT64 file_size(EFI_FILE_HANDLE fileHandle)
{
	UINT64		   size;
	EFI_FILE_INFO *fileInfo;
	fileInfo = LibFileInfo(fileHandle);
	size = fileInfo->FileSize;
	FreePool(fileInfo);
	return size + 0x8;
}

EFI_STATUS load_elf_segment(EFI_FILE *kernel_img, EFI_PHYSICAL_ADDRESS segment_offset, UINTN segment_size,
							UINTN segment_mem_size, EFI_PHYSICAL_ADDRESS segment_physical_addr)
{
	EFI_STATUS			 status;
	VOID				*segment_buffer = NULL;
	UINTN				 buffer_read_size = 0;
	UINTN				 segment_page_count = EFI_SIZE_TO_PAGES(segment_size);
	EFI_PHYSICAL_ADDRESS zero_fill_start = 0;
	UINTN				 zero_fill_count = 0;

	status = uefi_call_wrapper(kernel_img->SetPosition, 2, kernel_img, segment_offset);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to set file position: %r\n", status);
		return status;
	}

	EFI_PHYSICAL_ADDRESS dest_addr = segment_physical_addr;
	Print(L"Loading segment at address: 0x%llx\n", dest_addr);
	status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, segment_page_count, &dest_addr);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to allocate pages: %r\n", status);
		return status;
	}

	if (segment_size > 0)
	{
		buffer_read_size = segment_size;
		status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, buffer_read_size, (VOID **)&segment_buffer);
		status = uefi_call_wrapper(kernel_img->Read, 3, kernel_img, &buffer_read_size, (VOID *)segment_buffer);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to read kernel segment: %r\n", status);
			return status;
		}
		status = uefi_call_wrapper(BS->CopyMem, 3, dest_addr, segment_buffer, segment_size);
		status = uefi_call_wrapper(BS->FreePool, 1, segment_buffer);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to free segment buffer: %r\n", status);
			return status;
		}
	}

	zero_fill_start = dest_addr + segment_size;
	zero_fill_count = segment_mem_size - segment_size;

	if (zero_fill_count > 0)
	{
		status = uefi_call_wrapper(BS->SetMem, 3, zero_fill_start, zero_fill_count, 0);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to set memory: %r\n", status);
			return status;
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS load_kernel_segments(EFI_FILE *kernel_img, VOID *kernel_header_buffer, VOID *kernel_program_headers_buffer)
{
	EFI_STATUS status;
	UINT16	   count_program_headers = 0;
	UINT16	   count_segment_loaded = 0;
	UINT16	   i = 0;

	Print(L"Program header count: %d\n", ((Elf64_Ehdr *)kernel_header_buffer)->e_phnum);
	count_program_headers = ((Elf64_Ehdr *)kernel_header_buffer)->e_phnum;
	if (count_program_headers == 0)
	{
		Print(L"Invalid program header count\n");
		return EFI_INVALID_PARAMETER;
	}

	Elf64_Phdr *program_headers = (Elf64_Phdr *)kernel_program_headers_buffer;
	for (i = 0; i < count_program_headers; i++)
	{
		if (program_headers[i].p_type == PT_LOAD)
		{
			status = load_elf_segment(kernel_img, program_headers[i].p_offset, program_headers[i].p_filesz,
									  program_headers[i].p_memsz, program_headers[i].p_paddr);
			if (EFI_ERROR(status))
			{
				Print(L"Failed to load ELF segment: %r\n", status);
				return status;
			}
			count_segment_loaded++;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(EFI_HANDLE image, CHAR16 *kernel_file_name, EFI_VIRTUAL_ADDRESS *kernel_entry_point)
{
	EFI_STATUS status;
	EFI_FILE  *kernel_img = NULL;
	VOID	  *kernel_header = NULL;
	VOID	  *kernel_program_headers = NULL;
	UINT8	  *elf_identiy_buffer = NULL;

	EFI_FILE *root_vol_sys = open_volume(image);
	Print(L"Searching for kernel file...\n");
	status = uefi_call_wrapper(root_vol_sys->Open, 5, root_vol_sys, &kernel_img, kernel_file_name, EFI_FILE_MODE_READ,
							   EFI_FILE_READ_ONLY);
	Print(L"Kernel file opened...\n");
	status = read_elf_identity(kernel_img, &elf_identiy_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF identity: %r\n", status);
		return status;
	}
	status = validate_elf(elf_identiy_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Invalid ELF file: %r\n", status);
		return status;
	}
	status = uefi_call_wrapper(BS->FreePool, 1, elf_identiy_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to free ELF identity buffer: %r\n", status);
		return status;
	}
	status = read_elf_file(kernel_img, &kernel_header, &kernel_program_headers);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF file: %r\n", status);
		return status;
	}

	*kernel_entry_point = ((Elf64_Ehdr *)kernel_header)->e_entry;
	Print(L"  Entry point:              0x%llx\n", ((Elf64_Ehdr *)kernel_header)->e_entry);
	Print(L"  Program header offset:    0x%llx\n", ((Elf64_Ehdr *)kernel_header)->e_phoff);
	Print(L"  Section header count:     %u\n", ((Elf64_Ehdr *)kernel_header)->e_shnum);
	Print(L"Kernel header: %p\n", kernel_header);
	status = load_kernel_segments(kernel_img, kernel_header, kernel_program_headers);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to load kernel segments: %r\n", status);
		return status;
	}

	status = uefi_call_wrapper(kernel_img->Close, 1, kernel_img);
	status = uefi_call_wrapper(BS->FreePool, 1, (VOID *)kernel_program_headers);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to free kernel program headers: %r\n", status);
		return status;
	}

	return EFI_SUCCESS;
}
