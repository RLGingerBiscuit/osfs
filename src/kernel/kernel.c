#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include <kernel/tty.h>

void kernel_main() {
  gdt_init();

  tty_init();

  printf("Hello %s World\n", "Kernel");
  printf("The number is: %d\n", 42);
}
