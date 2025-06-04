#include <stddef.h>
#include <stdint.h>

#include "vga.h"

volatile uint16_t *vga_buffer = (uint16_t *)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

static int term_col = 0;
static int term_row = 0;
static uint8_t term_color = 0x0F;

void vga_init(void) {
  for (int row = 0; row < VGA_ROWS; ++row) {
    for (int col = 0; col < VGA_COLS; ++col) {
      const size_t index = (row * VGA_COLS) + col;
      vga_buffer[index] = ((uint8_t)term_color << 8) | ' ';
    }
  }
}

void vga_putc(char c) {
  switch (c) {
  case '\n': {
    term_col = 0;
    term_row++;
  }; break;
  default: {
    const size_t index = (term_row * VGA_COLS) + term_col;
    vga_buffer[index] = ((uint16_t)term_color << 8) | c;
    term_col++;
  }; break;
  }

  if (term_col >= VGA_COLS) {
    term_col = 0;
    term_row++;
  }

  if (term_row >= VGA_ROWS) {
    term_col = 0;
    term_row = 0;
  }
}

void vga_print(const char *str) {
  for (const char *c = str; *str; c = ++str) {
    vga_putc(*c);
  }
}
