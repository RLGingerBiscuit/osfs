#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include <kernel/multiboot.h>
#include <kernel/tty.h>

multiboot_info_t *multiboot_info_ptr;

void kernel_main(multiboot_info_t *mbd) {
  gdt_init();

  tty_init();

  printf("Hello %s World\n", "Kernel");
  printf("The number is %d\n", 42);
  printf("Bootloader: %s\n", (char *)multiboot_info_ptr->boot_loader_name);
  printf("Multiboot: %p\n", multiboot_info_ptr);
}
