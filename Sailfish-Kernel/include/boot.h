#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

/*
	SAILFISH_EFI structs are kernel accesable data types from EFI without linking EFI objects to the kernel after
   compilation
*/

typedef uint64_t SAILFISH_EFI_PHYSICAL_ADDRESS;
typedef uint64_t SAILFISH_EFI_VIRTUAL_ADDRESS;

typedef struct
{
	uint64_t ptr;
	uint64_t size;
} __attribute__((packed)) MEMORY_MAP_ENTRY;

typedef struct
{
	uint32_t					  Type;
	uint32_t					  Pad;
	SAILFISH_EFI_PHYSICAL_ADDRESS PhysicalStart;
	SAILFISH_EFI_VIRTUAL_ADDRESS  VirtualStart;
	uint64_t					  NumberOfPages;
	uint64_t					  Attribute;
} SAILFISH_EFI_MEMORY_DESCRIPTOR;

typedef enum
{
	SF_EfiReservedMemoryType,
	SF_EfiLoaderCode,
	SF_EfiLoaderData,
	SF_EfiBootServicesCode,
	SF_EfiBootServicesData,
	SF_EfiRuntimeServicesCode,
	SF_EfiRuntimeServicesData,
	SF_EfiConventionalMemory,
	SF_EfiUnusableMemory,
	SF_EfiACPIReclaimMemory,
	SF_EfiACPIMemoryNVS,
	SF_EfiMemoryMappedIO,
	SF_EfiMemoryMappedIOPortSpace,
	SF_EfiPalCode,
	SF_EfiPersistentMemory,
	SF_EfiUnacceptedMemoryType,
	SF_EfiMaxMemoryType
} SAILFISH_EFI_MEMORY_TYPE;

typedef struct
{
	uint32_t size;

	uint8_t	 num_cores;
	uint16_t bspid;
	uint16_t timezone;
	uint8_t	 datetime[8];

	uint64_t initrd_ptr;
	uint64_t initrd_size;

	uint8_t	 fb_type;
	uint64_t fb_addr;
	uint64_t fb_size;
	uint32_t fb_width;
	uint32_t fb_height;
	uint32_t fb_bpp;
	uint32_t fb_scanline;

	uint64_t mmap_addr;
	uint64_t mmap_size;
	uint64_t mmap_desc_size;

	union
	{
		uint64_t acpi_ptr;
		uint64_t smbi_ptr;
		uint64_t efi_ptr;
		uint64_t mp_ptr;
		uint64_t unused0;
		uint64_t unused1;
		uint64_t unused2;
		uint64_t unused3;
	} x86_64;

	MEMORY_MAP_ENTRY mmap;
} __attribute__((packed)) KERNEL_SETTINGS;

#endif