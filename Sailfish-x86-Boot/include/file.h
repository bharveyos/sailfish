#ifndef FILE_H
#define FILE_H

#include "logger.h"
#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_STATUS		read_file(EFI_HANDLE image, CHAR16 *kernel_config_file_name);
EFI_FILE_HANDLE open_volume(EFI_HANDLE handle);
UINT64			file_size(EFI_FILE_HANDLE fileHandle);
EFI_STATUS		load_kernel_image(EFI_HANDLE image, CHAR16 *kernel_file_name, EFI_VIRTUAL_ADDRESS *kernel_entry_point);
EFI_STATUS load_kernel_segments(EFI_FILE *kernel_img, VOID *kernel_header_buffer, VOID *kernel_program_headers_buffer);
EFI_STATUS load_elf_segment(EFI_FILE *kernel_img, EFI_PHYSICAL_ADDRESS segment_offset, UINTN segment_size,
							UINTN segment_mem_size, EFI_PHYSICAL_ADDRESS segment_physical_addr);

#endif