#include "acpi.h"
#include "boot.h"
#include "device.h"
#include "file.h"
#include "framebuffer.h"
#include "mmap.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable)
{
	InitializeLib(imageHandle, systemTable);

	// UEFI Structs
	EFI_STATUS					  status;
	EFI_MEMORY_DESCRIPTOR		 *memoryMap = NULL;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutput = NULL;

	// Custom Structs
	DeviceList		*deviceList = NULL;
	KERNEL_SETTINGS *kernelSettings = AllocatePool(sizeof(KERNEL_SETTINGS));

	// File names
	CHAR16 *name = L"EFI\\Sailfish\\DEMO.txt";
	CHAR16 *kernelName = L"EFI\\Sailfish\\kernel.elf";

	UINTN				 mmapKey = 0;
	EFI_PHYSICAL_ADDRESS kernelEntryPoint;

	VOID *acpi = NULL;

	status = uefi_call_wrapper(BS->SetWatchdogTimer, 4, 0, 0, 0, NULL);
	uefi_call_wrapper(systemTable->ConOut->SetAttribute, 2, systemTable->ConOut, EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK));
	uefi_call_wrapper(systemTable->ConOut->ClearScreen, 1, systemTable->ConOut);
	if (EFI_ERROR(status))
		return status;
	Print(L"Launching Baobab x86 UEFI\r\n");

	Print(L"Preparing to read config file\n");
	status = read_file(imageHandle, name);
	status = load_kernel_image(imageHandle, kernelName, &kernelEntryPoint);

	get_device_list(&deviceList);
	Print(L"Devices found: %d\n", deviceList->count);
	Print(L"First device storage size: %d\n", deviceList->device_list[0].Block.capacity);

	status = init_framebuffer(&graphicsOutput);
	if (!EFI_ERROR(status))
	{
		print_framebuffer_info(graphicsOutput);
	}

	acpi = get_acpi_table();

	if (kernelSettings == NULL)
		return EFI_OUT_OF_RESOURCES;

	kernelSettings->fb_type = graphicsOutput->Mode->Info->PixelFormat;
	kernelSettings->fb_addr = (UINT64)graphicsOutput->Mode->FrameBufferBase;
	kernelSettings->fb_size = graphicsOutput->Mode->FrameBufferSize;
	kernelSettings->fb_width = graphicsOutput->Mode->Info->HorizontalResolution;
	kernelSettings->fb_height = graphicsOutput->Mode->Info->VerticalResolution;
	kernelSettings->fb_scanline = graphicsOutput->Mode->Info->PixelsPerScanLine;

	status = get_mmap(&memoryMap, kernelSettings, &mmapKey);

	status = exit_boot_services(imageHandle, mmapKey);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to exit boot services: %r\n", status);
		return status;
	}

	kernelSettings->mmap_addr = (UINT64)memoryMap;
	kernelSettings->mmap_size = (*kernelSettings).mmap_size;
	kernelSettings->mmap_desc_size = (*kernelSettings).mmap_desc_size;
	kernelSettings->x86_64.acpi_ptr = (UINT64)acpi;

	typedef void (*kernel_entry_t)(KERNEL_SETTINGS *);
	kernel_entry_t entry = (kernel_entry_t)kernelEntryPoint;
	entry(kernelSettings);

	Print(L"Kernel returned unexpectedly\n");
	return EFI_LOAD_ERROR;
}