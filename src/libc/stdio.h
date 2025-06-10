#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stdarg.h>

int vprintf(const char *restrict fmt, va_list ap);
int printf(const char *restrict fmt, ...);

#endif // LIBC_STDIO_H
