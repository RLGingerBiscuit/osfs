#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/panic.h>
#include <kernel/vga.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(void) {
  vga_init();

  printf("Hello: %% %u %d %i %o %x %X %p\n", 42, -42, 42, 42, 42, 42, 42);

  // vga_print("Hello World!\nWe're kerneling this thang!\n");
  // for (int i = 0; i < 256; ++i) {
  //   vga_setcol(i);
  //   vga_putc('M');
  // }

  for (int i = 33; i < 33 + 40; ++i) {
    for (int j = 0; j < 80; ++j) {
      vga_putc(i);
    }
    size_t x = 0;
    while (x < 25000000) {
      x++;
    }
  }
}
