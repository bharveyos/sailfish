#include "boot.h"
#include <assert.h>
#include <efi.h>
#include <efilib.h>
#include <mmap.h>

#define EFI_PAGE_SIZE 4096

EFI_STATUS get_mmap(EFI_MEMORY_DESCRIPTOR **memoryMap, KERNEL_SETTINGS *kernelSettings, UINTN *memoryMapKey)
{
	EFI_STATUS status;
	UINTN	   memoryMapSize = 0;
	UINTN	   descriptorSize;
	UINT32	   descriptorVersion;

	EFI_MEMORY_DESCRIPTOR *tempMap = NULL;

	// First call to get the correct buffer size
	status =
		uefi_call_wrapper(BS->GetMemoryMap, 5, &memoryMapSize, NULL, memoryMapKey, &descriptorSize, &descriptorVersion);
	if (status != EFI_BUFFER_TOO_SMALL)
	{
		Print(L"Failed to get memory map size: %r\n", status);
		return status;
	}

	// Add extra space for new entries between GetMemoryMap and ExitBootServices
	memoryMapSize += 2 * descriptorSize;

	// Allocate buffer
	status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, memoryMapSize, (VOID **)&tempMap);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to allocate memory map buffer: %r\n", status);
		return status;
	}

	// Get the memory map for real
	status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memoryMapSize, tempMap, memoryMapKey, &descriptorSize,
							   &descriptorVersion);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to get memory map: %r\n", status);
		return status;
	}

	// Populate the KERNEL_SETTINGS structure
	*memoryMap = tempMap;
	kernelSettings->mmap_size = memoryMapSize;
	kernelSettings->mmap_desc_size = descriptorSize;

	return EFI_SUCCESS;
}

UINT64 get_usable_memory(EFI_MEMORY_DESCRIPTOR *memoryMap, UINTN memoryMapSize, UINTN descriptorSize)
{
	UINT64 totalMemory = 0;
	UINTN  numberOfEntries = memoryMapSize / descriptorSize;
	Print(L"Entries in memory map: %d\n", numberOfEntries);

	for (UINTN i = 0; i < numberOfEntries; i++)
	{
		EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)memoryMap + (i * descriptorSize));
		if (desc->Type == EfiConventionalMemory)
		{
			totalMemory += desc->NumberOfPages * EFI_PAGE_SIZE;
		}
	}

	return totalMemory;
}

EFI_STATUS exit_boot_services(EFI_HANDLE ImageHandle, UINTN memoryMapKey)
{
	return uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, memoryMapKey);
}

EFI_STATUS print_memory_info(KERNEL_SETTINGS *kernelSettings, EFI_MEMORY_DESCRIPTOR *memoryMap)
{
	UINT64 totalMemory = 0;
	Print(L"Memory map address: %p\n", memoryMap);
	Print(L"Memory map size: %d\n", (*kernelSettings).mmap_size);
	Print(L"Memory map descriptor size: %d\n", (*kernelSettings).mmap_desc_size);
	totalMemory = get_usable_memory(memoryMap, (*kernelSettings).mmap_size, (*kernelSettings).mmap_desc_size);
	Print(L"Total usable RAM: %lu bytes (%lu MB)\n", totalMemory, totalMemory / 1024 / 1024);
	return EFI_SUCCESS;
}