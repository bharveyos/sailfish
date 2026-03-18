#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <efi.h>
#include <efilib.h>

EFI_STATUS init_framebuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL **graphicsOutput);
void	   print_framebuffer_info(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutput);

#endif