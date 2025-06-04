#ifndef OSFS_VGA_H
#define OSFS_VGA_H

#include <stdint.h>

enum vga_colour {
  VGA_BLACK = 0x0,
  VGA_BLUE = 0x1,
  VGA_GREEN = 0x2,
  VGA_CYAN = 0x3,
  VGA_RED = 0x4,
  VGA_MAGENTA = 0x5,
  VGA_ORANGE = 0x6,
  VGA_LIGHT_GRAY = 0x7,
  VGA_DARK_GRAY = 0x8,
  VGA_LIGHT_BLUE = 0x9,
  VGA_LIGHT_GREEN = 0xa,
  VGA_LIGHT_CYAN = 0xb,
  VGA_LIGHT_RED = 0xc,
  VGA_LIGHT_MAGENTA = 0xd,
  VGA_LIGHT_YELLOW = 0xe,
  VGA_WHITE = 0xf,
};

void vga_init(void);

void vga_setcol(enum vga_colour col);
void vga_setfg(enum vga_colour fg);
void vga_setbg(enum vga_colour bg);

void vga_putc(char c);

void vga_print(const char *str);

#endif // OSFS_VGA_H
