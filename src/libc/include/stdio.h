#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

int vprintf(const char *restrict fmt, va_list ap)
    __attribute__((format(printf, 1, 0)));
int printf(const char *restrict fmt, ...) __attribute__((format(printf, 1, 2)));

int putchar(int ch);

int puts(const char *str);

#endif // _STDIO_H
