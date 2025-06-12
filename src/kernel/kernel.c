#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include <kernel/multiboot.h>
#include <kernel/tty.h>

void kernel_main(multiboot_info_t *mbd) {
  gdt_init();

  tty_init();

  printf("Hello %s World\n", "Kernel");
  printf("The number is %d\n", 42);
  printf("Bootloader: %s\n", (char*)mbd->boot_loader_name);
}
