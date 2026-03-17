#ifndef LOGGER_H
#define LOGGER_H

#include <efi.h>

#if !defined(NDEBUG)
#define BOOTLOADER_DEBUG
#endif

UINTN DEBUG_PRINT(CHAR16 *msg);

#endif