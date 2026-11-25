#include "device.h"
#include <efi.h>
#include <efilib.h>

VOID get_device_list(DeviceList **storageDevices)
{
	EFI_STATUS	status;
	UINTN		numberOfHandles;
	UINTN		validDeviceCount = 0;
	EFI_HANDLE *handleBuffer = NULL;

	EFI_DEVICE_PATH_PROTOCOL	*devicePath = NULL;
	EFI_BLOCK_IO_PROTOCOL		*blockStorage = NULL;
	EFI_SIMPLE_NETWORK_PROTOCOL *networkSystem = NULL;

	DeviceItem deviceItem = {0};
	*storageDevices = AllocateZeroPool(sizeof(DeviceList));
	if (*storageDevices == NULL)
	{
		Print(L"Memory allocation for device list failed\n");
	}
	(*storageDevices)->count = 0;

	status = uefi_call_wrapper(BS->LocateHandleBuffer, 5, AllHandles, NULL, NULL, &numberOfHandles, &handleBuffer);
	if (!EFI_ERROR(status))
	{
		for (int i = 0; i < numberOfHandles; i++)
		{
			if (EFI_ERROR(uefi_call_wrapper(BS->HandleProtocol, 3, handleBuffer[i], &gEfiDevicePathProtocolGuid,
											(VOID **)&devicePath)))
				continue;

			/* Get storage devices */
			status = uefi_call_wrapper(BS->HandleProtocol, 3, handleBuffer[i], &gEfiBlockIoProtocolGuid,
									   (VOID **)&blockStorage);
			if (!EFI_ERROR(status) && blockStorage->Media->MediaPresent)
			{
				deviceItem.handle = handleBuffer[i];
				deviceItem.type = DeviceTypeBlock;
				deviceItem.device_path_str = DevicePathToStr(devicePath);
				deviceItem.protocol_interface = blockStorage;
				deviceItem.protocol_guid = gEfiBlockIoProtocolGuid;

				deviceItem.Block.media_id = blockStorage->Media->MediaId;
				deviceItem.Block.capacity =
					((blockStorage->Media->LastBlock + 1) * blockStorage->Media->BlockSize) / 1024;
				deviceItem.Block.removable = blockStorage->Media->RemovableMedia;

				(*storageDevices)->device_list[validDeviceCount] = deviceItem;
				(*storageDevices)->count += 1;
				Print(L"Found Block Device %u: %s (%d KiB)\n", validDeviceCount, DevicePathToStr(devicePath),
					  deviceItem.Block.capacity);
				validDeviceCount++;
			}

			/* Get network devices */
			status = uefi_call_wrapper(BS->HandleProtocol, 3, handleBuffer[i], &gEfiSimpleNetworkProtocolGuid,
									   (VOID **)&networkSystem);
			if (!EFI_ERROR(status))
			{
				deviceItem.handle = handleBuffer[i];
				deviceItem.type = DeviceTypeNetwork;
				deviceItem.device_path_str = DevicePathToStr(devicePath);
				deviceItem.protocol_interface = networkSystem;
				deviceItem.protocol_guid = gEfiSimpleNetworkProtocolGuid;

				deviceItem.Network.state = networkSystem->Mode->State;
				deviceItem.Network.hw_address_size = networkSystem->Mode->HwAddressSize;
				deviceItem.Network.media_header_size = networkSystem->Mode->MediaHeaderSize;
				deviceItem.Network.max_packet_size = networkSystem->Mode->MaxPacketSize;
				deviceItem.Network.perminent_address = networkSystem->Mode->PermanentAddress;
				deviceItem.Network.current_address = networkSystem->Mode->CurrentAddress;
				deviceItem.Network.media_present = networkSystem->Mode->MediaPresent;

				(*storageDevices)->device_list[validDeviceCount] = deviceItem;
				(*storageDevices)->count += 1;
				Print(L"Found Network Device %u: %s\n", validDeviceCount, DevicePathToStr(devicePath));
				validDeviceCount++;
			}
		}
	}
}
