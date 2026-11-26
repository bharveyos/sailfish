#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

void clear_screen(uint32_t *fb, uint32_t width, uint32_t height, uint32_t scanline, uint16_t color);

#endif