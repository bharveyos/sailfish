#include "screen.h"

void clear_screen(uint32_t *fb, uint32_t width, uint32_t height, uint32_t scanline, uint32_t color)
{
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			fb[y * scanline + x] = color;
		}
	}
}
