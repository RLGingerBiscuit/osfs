#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "vga.h"
#include <kernel/tty.h>

void tty_init(void) { vga_init(); }

void tty_putc(char c) { vga_putc(c); }

void tty_print(const char *str, size_t len) { vga_print(str, len); }

void tty_printstr(const char *str) { vga_print(str, strlen(str)); }

void tty_setpos(int x, int y) { vga_setpos(x, y); }
void tty_modpos(int x, int y) { vga_modpos(x, y); }
