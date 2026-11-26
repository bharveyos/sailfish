#ifndef ASCII_H
#define ASCII_H

#include <stdint.h>

extern uint32_t *fb;
extern uint32_t	 width;
extern uint32_t	 height;
extern uint32_t	 scanline;

extern int		kprint_y;
extern uint64_t kprint_x;

void draw_char(uint32_t *fb, uint32_t scanline, int x, int y, char c, uint32_t color);
void draw_string(uint32_t *fb, uint32_t scanline, int x, int y, const char *str, uint32_t color);
void kprint(const char *str, ...);

#endif
