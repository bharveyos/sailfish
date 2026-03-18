#include "serial.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS configure_serial_port(EFI_SERIAL_IO_PROTOCOL *serialIo)
{
	EFI_STATUS		   status;
	UINT64			   baudRate = 115200;
	UINT32			   receiveFifoDepth = 0;
	UINT32			   timeout = 0;
	EFI_PARITY_TYPE	   parityType = DefaultParity;
	UINT8			   dataBits = 8;
	EFI_STOP_BITS_TYPE stopBits = DefaultStopBits;

	status = uefi_call_wrapper(serialIo->SetAttributes, 7, serialIo, baudRate, receiveFifoDepth, timeout, parityType,
							   dataBits, stopBits);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to configure serial port: %r\n", status);
		return status;
	}

	return EFI_SUCCESS;
}

EFI_STATUS init_serial_port()
{
	EFI_STATUS status;
	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiSerialIoProtocolGuid, NULL, &serialProtocol.serial_protocol);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to locate Serial IO Protocol: %r\n", status);
		return status;
	}

	status = configure_serial_port(serialProtocol.serial_protocol);
	return EFI_SUCCESS;
}

EFI_STATUS print_serial_port(EFI_SERIAL_IO_PROTOCOL *serialIo, CHAR16 *message)
{
	EFI_STATUS status;
	UINTN	   buffer;
	UINTN	   messageLength = StrLen(message);

	if (messageLength > MAX_SERIAL_MESSAGE_LENGTH)
	{
		Print(L"Message length exceeds maximum allowed length\n");
		return EFI_BAD_BUFFER_SIZE;
	}

	buffer = messageLength * 2;
	status = uefi_call_wrapper(serialIo->Write, 3, serialIo, &buffer, (VOID *)message);
	if (EFI_ERROR(status))
	{
		Print(L"Failed to write to serial port: %r\n", status);
		return status;
	}
	return EFI_SUCCESS;
}