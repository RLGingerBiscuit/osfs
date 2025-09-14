
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"
#include "vga.h"
#include <kernel/tty.h>

const int VGA_COLS = 80;
const int VGA_ROWS = 25;

static int term_col = 0;
static int term_row = 0;
static uint8_t term_colour = 0x0f;
volatile uint16_t *vga_buffer =
    (volatile uint16_t *)(KERNEL_VIRT_BASE |
                          PAGE_SIZE * (PAGE_SIZE / 4 - 1)); // Virtual address

void vga_init(void) {
  for (int col = 0; col < VGA_COLS; ++col) {
    vga_buffer[col] = ((uint16_t)term_colour << 8) | ' ';
  }
  for (int row = 1; row < VGA_ROWS; ++row) {
    memcpy((uint16_t *)&vga_buffer[row * VGA_COLS], (uint16_t *)vga_buffer,
           sizeof(uint16_t) * VGA_COLS);
  }
}

inline void vga_setcol(enum vga_colour col) { term_colour = col; }

inline void vga_setfg(enum vga_colour fg) {
  term_colour = (term_colour & 0xf0) | (fg & 0xf);
}

inline void vga_setbg(enum vga_colour bg) {
  term_colour = (bg & 0xf) << 4 | (term_colour & 0xf);
}

static inline void vga_advance_line() {
  term_col = 0;
  if (term_row >= VGA_ROWS - 1) {
    // scroll
    memcpy((uint16_t *)vga_buffer, (uint16_t *)&vga_buffer[VGA_COLS],
           sizeof(uint16_t) * (VGA_COLS * (VGA_ROWS - 1)));
    for (int col = 0; col < VGA_COLS; ++col)
      vga_buffer[(VGA_ROWS - 1) * (VGA_COLS) + col] = ' ';
    term_row = VGA_ROWS - 1;
  } else {
    // just go down
    term_row++;
  }
}

void vga_putc(char c) {
  switch (c) {
  case '\t': {
    term_col += 4 - (term_col % 4);
  }; break;
  case '\n': {
    vga_advance_line();
  }; break;
  default: {
    const size_t index = (term_row * VGA_COLS) + term_col;
    vga_buffer[index] = ((uint16_t)term_colour << 8) | c;
    term_col++;
  }; break;
  }

  if (term_col >= VGA_COLS || term_row >= VGA_ROWS)
    vga_advance_line();
}

void vga_print(const char *str, size_t len) {
  for (size_t i = 0; i < len; ++i)
    vga_putc(str[i]);
}

void vga_setpos(int x, int y) {
  if (x < 0)
    term_col = 0;
  else if (x >= VGA_COLS)
    term_col = VGA_COLS - 1;
  else
    term_col = x;

  if (y < 0)
    term_row = 0;
  else if (y >= VGA_ROWS)
    term_row = VGA_ROWS - 1;
  else
    term_row = y;
}

void vga_modpos(int x, int y) { vga_setpos(term_col + x, term_row + y); }
