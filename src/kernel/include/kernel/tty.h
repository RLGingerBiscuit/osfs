#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include <stddef.h>
#include <stdint.h>

void tty_init(void);
void tty_putc(char c);
void tty_print(const char *str, size_t len);
void tty_printstr(const char *str);
void tty_setpos(int x, int y);
void tty_modpos(int x, int y);

#endif // _KERNEL_VGA_H
