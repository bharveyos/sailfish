
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
	clear_screen(fb, width, height, scanline, VGA_COLOR(0x04, BACKGROUND_BLACK));
	kprint("We are printing from kernel\n");
	kprint("This is a new line\n");
	kprint("No previous new line so it's on the same line\n");
	kerror("And the kernel is panicing!");
	while (1)
	{
	}
	return 0;
}