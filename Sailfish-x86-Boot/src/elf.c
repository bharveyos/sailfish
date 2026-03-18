#include "elf.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS read_elf_file(EFI_FILE *elf_file, VOID **elf_header_buffer, VOID **elf_program_header_buffer)
{
	EFI_STATUS status;
	UINTN	   buffer_size = 0;
	UINT64	   program_header_offset = 0;

	status = uefi_call_wrapper(elf_file->SetPosition, 2, elf_file, 0);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to set file position: %r\n", status);
		return status;
	}

	buffer_size = sizeof(Elf64_Ehdr);
	status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, buffer_size, elf_header_buffer);
	status = uefi_call_wrapper(elf_file->Read, 3, elf_file, &buffer_size, *elf_header_buffer);
	Print(L"ELF header size: %d\n", buffer_size);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF header: %r\n", status);
		return status;
	}

	program_header_offset = ((Elf64_Ehdr *)*elf_header_buffer)->e_phoff;
	buffer_size = sizeof(Elf64_Phdr) * ((Elf64_Ehdr *)*elf_header_buffer)->e_phnum;
	Print(L"Program header offset: %d\n", program_header_offset);
	status = uefi_call_wrapper(elf_file->SetPosition, 2, elf_file, program_header_offset);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to set file position: %r\n", status);
		return status;
	}

	status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, buffer_size, elf_program_header_buffer);
	status = uefi_call_wrapper(elf_file->Read, 3, elf_file, &buffer_size, *elf_program_header_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF program header: %r\n", status);
		return status;
	}

	return EFI_SUCCESS;
}

EFI_STATUS read_elf_identity(EFI_FILE *elf_file, UINT8 **elf_identity_buffer)
{
	UINT8	   buffer_read_size = 16;
	EFI_STATUS status;

	status = uefi_call_wrapper(elf_file->SetPosition, 2, elf_file, 0);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to set file position: %r\n", status);
		return status;
	}

	status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, buffer_read_size, (VOID **)elf_identity_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to allocate memory for ELF identity: %r\n", status);
		return status;
	}

	status = uefi_call_wrapper(elf_file->Read, 3, elf_file, &buffer_read_size, (VOID *)*elf_identity_buffer);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to read ELF identity: %r\n", status);
		return status;
	}

	return EFI_SUCCESS;
}

EFI_STATUS validate_elf(UINT8 *elf_identity_buffer)
{
	Print(L"Validating ELF file...\n");
	if (elf_identity_buffer[0] != 0x7F || elf_identity_buffer[1] != 'E' || elf_identity_buffer[2] != 'L' ||
		elf_identity_buffer[3] != 'F')
	{
		Print(L"Invalid ELF file\n");
		return EFI_INVALID_PARAMETER;
	}

	if (elf_identity_buffer[4] != ELFCLASS64)
	{
		Print(L"Unsupported ELF class\n");
		return EFI_UNSUPPORTED;
	}

	if (elf_identity_buffer[5] != ELFDATA2LSB)
	{
		Print(L"Unsupported ELF data encoding\n");
		return EFI_UNSUPPORTED;
	}

	return EFI_SUCCESS;
}