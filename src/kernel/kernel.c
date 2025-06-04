#include <stddef.h>
#include <stdint.h>

#include <kernel/vga.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(void) {
  vga_init();

  // vga_print("Hello World!\nWe're kerneling this thang!\n");
  for (int i = 0; i < 256; ++i) {
    vga_setcol(i);
    vga_putc('M');
  }
}
