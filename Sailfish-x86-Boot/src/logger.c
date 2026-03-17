#include "logger.h"

UINTN DEBUG_PRINT(CHAR16 *msg)
{
	UINTN status = 0;
#ifdef BOOTLOADER_DEBUG
	status = Print(msg);
#endif
	return status;
}