
#include "ascii.h"
#include "boot.h"
#include "debug.h"
#include "screen.h"
#include "vga.h"
#include <stdint.h>

uint32_t *fb;
uint32_t  scanline;
uint32_t  height;
uint32_t  width;

int kernel_main(KERNEL_SETTINGS *settings)
{
	fb = (uint32_t *)(uint64_t)settings->fb_addr;
	scanline = settings->fb_scanline;
	height = settings->fb_height;
	width = settings->fb_width;
	clear_screen(fb, width, height, scanline, VGA_COLOR(FOREGROUND_BLACK, BACKGROUND_BLACK));
	kprint("We are printing from kernel\n");
	kerror("AHHHH Panicccc!");
	while (1)
	{
	}
	return 0;
}