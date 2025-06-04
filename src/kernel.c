#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

#include "vga.h"

void kernel_main(void) {
  vga_init();

  vga_print("Hello World!\nWe're kerneling this thang!\n");
}
