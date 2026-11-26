#include "ascii.h"
#include "vga.h"
#include <stdarg.h>
#include <stdint.h>

void kerror(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	while (*str != '\0')
	{
		if (*str == '\n')
		{
			kprint_x = 0;
			kprint_y += 10;
			++str;
			continue;
		}
		if (*str == '%')
		{
			str++;
			if (*str == 'd')
			{
				uint64_t num = va_arg(args, uint64_t);
				char	 num_buffer[21];
				int		 n = 20;
				num_buffer[n] = 0;
				if (num == 0)
				{
					num_buffer[--n] = '0';
				}
				else
				{
					uint64_t tmp = num;
					while (tmp > 0 && n > 0)
					{
						num_buffer[--n] = '0' + (tmp % 10);
						tmp /= 10;
					}
				}
				draw_string(fb, scanline, 16 + kprint_x, kprint_y, &num_buffer[n],
							VGA_COLOR(FOREGROUND_RED, BACKGROUND_BLACK));
				kprint_x += 8 * (20 - n);
				++str;
			}
			else if (*str == 'x')
			{
				uint64_t num = va_arg(args, uint64_t);
				char	 hex_buffer[17];
				int		 n = 16;
				hex_buffer[n] = 0;
				if (num == 0)
				{
					hex_buffer[--n] = '0';
				}
				else
				{
					uint64_t	tmp = num;
					const char *hex_digits = "0123456789ABCDEF";
					while (tmp > 0 && n > 0)
					{
						hex_buffer[--n] = hex_digits[tmp & 0xF];
						tmp >>= 4;
					}
				}
				draw_string(fb, scanline, 16 + kprint_x, kprint_y, &hex_buffer[n],
							VGA_COLOR(FOREGROUND_RED, BACKGROUND_BLACK));
				kprint_x += 8 * (16 - n);
				++str;
			}
			else if (*str == 'p')
			{
				uint64_t ptr = (uint64_t)va_arg(args, void *);
				char	 ptr_buffer[19];
				ptr_buffer[0] = '0';
				ptr_buffer[1] = 'x';
				const char *hex_digits = "0123456789ABCDEF";
				for (int i = 0; i < 16; ++i)
				{
					ptr_buffer[2 + i] = hex_digits[(ptr >> (4 * (15 - i))) & 0xF];
				}
				ptr_buffer[18] = 0;
				draw_string(fb, scanline, 16 + kprint_x, kprint_y, ptr_buffer,
							VGA_COLOR(FOREGROUND_RED, BACKGROUND_BLACK));
				kprint_x += 8 * 18;
				++str;
			}
		}
		else
		{
			draw_char(fb, scanline, 16 + kprint_x, kprint_y, *str, VGA_COLOR(FOREGROUND_RED, BACKGROUND_BLACK));
			kprint_x += 8;
			++str;
		}
	}
	va_end(args);
}