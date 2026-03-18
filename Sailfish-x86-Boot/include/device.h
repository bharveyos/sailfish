#ifndef DEVICE_H
#define DEVICE_H

#include <efi.h>
#include <efilib.h>

typedef enum
{
	DeviceTypeUnknown,
	DeviceTypeBlock,
	DeviceTypeNetwork,
	DeviceTypeUSB,
	DeviceTypeSerial,
	DeviceTypePCI,
	DeviceTypeGraphics
} DEVICE_TYPE;

typedef struct
{
	EFI_HANDLE	handle;
	DEVICE_TYPE type;
	CHAR16	   *device_path_str;
	VOID	   *protocol_interface;
	EFI_GUID	protocol_guid;
	union
	{
		UINT32	media_id;
		UINTN	capacity;
		BOOLEAN removable;
	} Block;
	union
	{
		UINT32			state;
		UINT32			hw_address_size;
		UINT32			media_header_size;
		UINT32			max_packet_size;
		EFI_MAC_ADDRESS perminent_address;
		EFI_MAC_ADDRESS current_address;
		BOOLEAN			media_present;
	} Network;
} DeviceItem;

typedef struct
{
	UINTN	   count;
	DeviceItem device_list[64];
} DeviceList;

VOID get_device_list(DeviceList **storageDevices);
#endif