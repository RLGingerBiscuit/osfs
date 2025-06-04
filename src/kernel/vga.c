#include <stddef.h>

#include <kernel/vga.h>

volatile uint16_t *vga_buffer = (uint16_t *)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

static int term_col = 0;
static int term_row = 0;
static uint8_t term_colour = 0x0f;

void vga_init(void) {
  for (int row = 0; row < VGA_ROWS; ++row) {
    for (int col = 0; col < VGA_COLS; ++col) {
      const size_t index = (row * VGA_COLS) + col;
      vga_buffer[index] = ((uint8_t)term_colour << 8) | ' ';
    }
  }
}

inline void vga_setcol(enum vga_colour col) { term_colour = col; }

inline void vga_setfg(enum vga_colour fg) { term_colour = fg & 0xf; }

inline void vga_setbg(enum vga_colour bg) { term_colour = (bg & 0xf) << 4; }

static inline void vga_advance_line() {
  term_col = 0;
  if (term_row >= VGA_ROWS-1) {
    // scroll
    for (int row = 1; row < VGA_ROWS; ++row) {
      for (int col = 0; col < VGA_COLS; ++col) {
        const size_t from_index = (row * VGA_COLS) + col;
        const size_t to_index = ((row - 1) * VGA_COLS) + col;
        vga_buffer[to_index] = vga_buffer[from_index];
      }
    }
    for (int col = 0; col < VGA_COLS; ++col) {
      const int row = VGA_ROWS - 1;
      const size_t index = (row * VGA_COLS) + col;
      vga_buffer[index] = ' ';
    }
    term_row = VGA_ROWS - 1;
  } else {
    // just go down
    term_row++;
  }
}

void vga_putc(char c) {
  switch (c) {
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

void vga_print(const char *str) {
  for (const char *c = str; *str; c = ++str) {
    vga_putc(*c);
  }
}
