#include "boot.h"
#include <stdint.h>

int kernel_main(KERNEL_SETTINGS *settings)
{
	// fb = (uint32_t *)(uint64_t)settings->fb_addr;
	// width = settings->fb_width;
	// height = settings->fb_height;
	// scanline = settings->fb_scanline;
	// clear_screen(fb, width, height, scanline, 0x00000000);
	while (1)
	{
	}
	return 0;
}