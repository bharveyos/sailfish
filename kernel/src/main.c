#include "boot.h"
#include "screen.h"
#include "vga.h"
#include <stdint.h>

int kernel_main(KERNEL_SETTINGS *settings)
{
	uint32_t *fb = (uint32_t *)(uint64_t)settings->fb_addr;
	uint32_t  scanline = settings->fb_scanline;
	uint32_t  height = settings->fb_height;
	uint32_t  width = settings->fb_width;
	// clear_screen(fb, width, height, scanline, VGA_COLOR(0x04, BACKGROUND_LIGHT_MAGENTA));
	while (1)
	{
	}
	return 0;
}