#ifndef MMAP_H
#define MMAP_H

#include "boot.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS get_mmap(EFI_MEMORY_DESCRIPTOR **memoryMap, KERNEL_SETTINGS *kernelSettings, UINTN *memoryMapKey);
UINT64	   get_usable_memory(EFI_MEMORY_DESCRIPTOR *memoryMap, UINTN memoryMapSize, UINTN descriptorSize);
EFI_STATUS exit_boot_services(EFI_HANDLE imageHandle, UINTN memoryMapKey);
EFI_STATUS print_memory_info(KERNEL_SETTINGS *kernelSettings, EFI_MEMORY_DESCRIPTOR *memoryMap);

#endif