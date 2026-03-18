#include "file.h"

EFI_STATUS read_file(EFI_HANDLE image, CHAR16 *kernelConfigFileName)
{
	EFI_FILE_HANDLE fileHandle;
	Print(L"Searching for file...\n");
	EFI_FILE_HANDLE rootVolume = open_volume(image);

	EFI_STATUS status =
		uefi_call_wrapper(rootVolume->Open, 5, rootVolume, &fileHandle, kernelConfigFileName, EFI_FILE_READ_ONLY, 0);

	if (EFI_ERROR(status))
	{
		Print(L"File could not be opened.\n");
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

	for (size_t i = 0; i <= readSize; i++)
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

EFI_STATUS load_elf_segment(EFI_FILE *kernelImg, EFI_PHYSICAL_ADDRESS segmentOffset, UINTN segmentSize,
							UINTN segmentMemSize, EFI_PHYSICAL_ADDRESS segmentPhysicalAddress)
{
	EFI_STATUS			 status;
	VOID				*segmentBuffer = NULL;
	UINTN				 bufferReadSize = 0;
	UINTN				 segmentPageCount = EFI_SIZE_TO_PAGES(segmentSize);
	EFI_PHYSICAL_ADDRESS zeroFillStart = 0;
	UINTN				 zeroFillCount = 0;

	status = uefi_call_wrapper(kernelImg->SetPosition, 2, kernelImg, segmentOffset);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to set file position: %r\n", status);
		return status;
	}

	EFI_PHYSICAL_ADDRESS destAddress = segmentPhysicalAddress;
	Print(L"Loading segment at address: 0x%llx\n", destAddress);
	status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, segmentPageCount, &destAddress);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to allocate pages: %r\n", status);
		return status;
	}

	if (segmentSize > 0)
	{
		bufferReadSize = segmentSize;
		status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, bufferReadSize, (VOID **)&segmentBuffer);
		status = uefi_call_wrapper(kernelImg->Read, 3, kernelImg, &bufferReadSize, (VOID *)segmentBuffer);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to read kernel segment: %r\n", status);
			return status;
		}
		status = uefi_call_wrapper(BS->CopyMem, 3, destAddress, segmentBuffer, segmentSize);
		status = uefi_call_wrapper(BS->FreePool, 1, segmentBuffer);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to free segment buffer: %r\n", status);
			return status;
		}
	}

	zeroFillStart = destAddress + segmentSize;
	zeroFillCount = segmentMemSize - segmentSize;

	if (zeroFillCount > 0)
	{
		status = uefi_call_wrapper(BS->SetMem, 3, zeroFillStart, zeroFillCount, 0);
		if (EFI_ERROR(status))
		{
			Print(L"Failed to set memory: %r\n", status);
			return status;
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS load_kernel_segments(EFI_FILE *kernelImg, VOID *kernelHeaderBuffer, VOID *kernelProgramHeadersBuffer)
{
	EFI_STATUS status;
	UINT16	   countProgramHeaders = 0;
	UINT16	   countSegmentLoaded = 0;
	UINT16	   i = 0;

	Print(L"Program header count: %d\n", ((Elf64_Ehdr *)kernelHeaderBuffer)->e_phnum);
	countProgramHeaders = ((Elf64_Ehdr *)kernelHeaderBuffer)->e_phnum;
	if (countProgramHeaders == 0)
	{
		Print(L"Invalid program header count\n");
		return EFI_INVALID_PARAMETER;
	}

	Elf64_Phdr *programHeaders = (Elf64_Phdr *)kernelProgramHeadersBuffer;
	for (i = 0; i < countProgramHeaders; i++)
	{
		if (programHeaders[i].p_type == PT_LOAD)
		{
			status = load_elf_segment(kernelImg, programHeaders[i].p_offset, programHeaders[i].p_filesz,
									  programHeaders[i].p_memsz, programHeaders[i].p_paddr);
			if (EFI_ERROR(status))
			{
				Print(L"Failed to load ELF segment: %r\n", status);
				return status;
			}
			countSegmentLoaded++;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(EFI_HANDLE image, CHAR16 *kernelFileName, EFI_VIRTUAL_ADDRESS *kernelEntryPoint)
{
	EFI_STATUS status;
	EFI_FILE  *kernelImg = NULL;
	VOID	  *kernelHeader = NULL;
	VOID	  *kernelProgramHeaders = NULL;
	UINT8	  *elfIdentityBuffer = NULL;

	EFI_FILE *rootSystemVolime = open_volume(image);
	Print(L"Searching for kernel file...\n");
	status = uefi_call_wrapper(rootSystemVolime->Open, 5, rootSystemVolime, &kernelImg, kernelFileName,
							   EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	Print(L"Kernel file opened...\n");
	status = read_elf_identity(kernelImg, &elfIdentityBuffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF identity: %r\n", status);
		return status;
	}
	status = validate_elf(elfIdentityBuffer);
	if (EFI_ERROR(status))
	{
		Print(L"Invalid ELF file: %r\n", status);
		return status;
	}
	status = uefi_call_wrapper(BS->FreePool, 1, elfIdentityBuffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to free ELF identity buffer: %r\n", status);
		return status;
	}
	status = read_elf_file(kernelImg, &kernelHeader, &kernelProgramHeaders);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF file: %r\n", status);
		return status;
	}

	*kernelEntryPoint = ((Elf64_Ehdr *)kernelHeader)->e_entry;
	Print(L"  Entry point:              0x%llx\n", ((Elf64_Ehdr *)kernelHeader)->e_entry);
	Print(L"  Program header offset:    0x%llx\n", ((Elf64_Ehdr *)kernelHeader)->e_phoff);
	Print(L"  Section header count:     %u\n", ((Elf64_Ehdr *)kernelHeader)->e_shnum);
	Print(L"Kernel header: %p\n", kernelHeader);
	status = load_kernel_segments(kernelImg, kernelHeader, kernelProgramHeaders);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to load kernel segments: %r\n", status);
		return status;
	}

	status = uefi_call_wrapper(kernelImg->Close, 1, kernelImg);
	status = uefi_call_wrapper(BS->FreePool, 1, (VOID *)kernelProgramHeaders);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to free kernel program headers: %r\n", status);
		return status;
	}

	return EFI_SUCCESS;
}
