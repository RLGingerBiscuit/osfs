#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/tty.h>

// FIXME: Find where to move this now
// #if defined(__linux__)
// #error "This code must be compiled with a cross-compiler"
// #elif !defined(__i386__)
// #error "This code must be compiled with an x86-elf compiler"
// #endif

void kernel_main() {
  tty_init();
  printf("Hello %s World\n", "Kernel");
  printf("The number is: %d\n", 42);
}
