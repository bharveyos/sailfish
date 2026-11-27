#include "memutils.h"

void *memcpy(void *dest, const void *src, size_t count)
{
	char	   *d = dest;
	const char *s = src;
	while (count--)
		*d++ = *s++;
	return dest;
}

void *memset(void *dest, int val, size_t count)
{
	unsigned char *ptr = dest;
	while (count-- > 0)
		*ptr++ = val;
	return dest;
}