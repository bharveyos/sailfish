#ifndef BOOT_H
#define BOOT_H

#include <efi.h>
#include <efilib.h>

typedef struct
{
	UINT64 ptr;
	UINT64 size;
} __attribute__((packed)) MEMORY_MAP_ENTRY;

typedef struct
{
	UINT32 size;

	UINT8  num_cores;
	UINT16 bspid;
	UINT16 timezone;
	UINT8  datetime[8];

	UINT64 initrd_ptr;
	UINT64 initrd_size;

	UINT8  fb_type;
	UINT64 fb_addr;
	UINT64 fb_size;
	UINT32 fb_width;
	UINT32 fb_height;
	UINT32 fb_bpp;
	UINT32 fb_scanline;

	UINT64 mmap_addr;
	UINT64 mmap_size;
	UINT64 mmap_desc_size;

	union
	{
		UINT64 acpi_ptr;
		UINT64 smbi_ptr;
		UINT64 efi_ptr;
		UINT64 mp_ptr;
		UINT64 unused0;
		UINT64 unused1;
		UINT64 unused2;
		UINT64 unused3;
	} x86_64;

	MEMORY_MAP_ENTRY mmap;
} __attribute__((packed)) KERNEL_SETTINGS;

#endif
