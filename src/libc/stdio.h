#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stdarg.h>

int vprintf(const char *restrict fmt, va_list ap)
    __attribute__((format(printf, 1, 0)));
int printf(const char *restrict fmt, ...) __attribute__((format(printf, 1, 2)));

#endif // LIBC_STDIO_H
