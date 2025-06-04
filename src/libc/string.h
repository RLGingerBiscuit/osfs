#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <stddef.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t count);
void *memset(void *restrict dest, int ch, size_t count);

#endif // LIBC_STRING_H
