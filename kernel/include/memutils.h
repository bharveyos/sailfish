#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, int val, size_t count);

#endif