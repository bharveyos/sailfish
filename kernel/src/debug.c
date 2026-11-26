#include "debug.h"
#include "ascii.h"

void kerror(const char *str, ...)
{
	uint32_t red = 0xFF0000;
	draw_string(fb, scanline, 16 + kprint_x, kprint_y, str, red);
	kprint_x += 8 * 16;
	++str;
}