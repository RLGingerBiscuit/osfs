#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "vga.h"
#include <kernel/tty.h>

void tty_init(void) { vga_init(); }

void tty_putc(char c) { vga_putc(c); }

void tty_print(const char *str, size_t len) { vga_print(str, len); }

void tty_printstr(const char *str) { vga_print(str, strlen(str)); }
