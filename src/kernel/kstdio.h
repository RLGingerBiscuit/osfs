#ifndef KERNEL_STDIO_H
#define KERNEL_STDIO_H

#include <stdarg.h>

int vga_vprintf(const char *restrict fmt, va_list ap)
    __attribute__((format(printf, 1, 0)));
int vga_printf(const char *restrict fmt, ...)
    __attribute__((format(printf, 1, 2)));

#endif // KERNEL_STDIO_H
