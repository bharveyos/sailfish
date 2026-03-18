#ifndef SERIAL_H
#define SERIAL_H

#include <efi.h>
#include <efilib.h>

#define MAX_SERIAL_MESSAGE_LENGTH 512

typedef struct
{
	EFI_SERIAL_IO_PROTOCOL *serial_protocol;
} SerialProtocol;

EFI_STATUS print_serial_port(EFI_SERIAL_IO_PROTOCOL *serialIo, CHAR16 *message);
EFI_STATUS init_serial_port(void);
EFI_STATUS configure_serial_port(EFI_SERIAL_IO_PROTOCOL *serialIo);

extern SerialProtocol serialProtocol;
#endif