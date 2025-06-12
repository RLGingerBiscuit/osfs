#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include <kernel/multiboot.h>
#include <kernel/tty.h>

void kernel_main(multiboot_info_t *mbd) {
  volatile char *vga = (volatile char *)0xC00B8000;
  vga[0] = 'X';
  vga[1] = 0x07;
  vga[2] = 'Y';
  vga[3] = 0x07;
  vga[4] = 'Z';
  vga[5] = 0x07;
  while (1) {
  }

  // gdt_init();

  // tty_init();

  // printf("Hello %s World\n", "Kernel");
  // printf("The number is %d\n", 42);
  // printf("Bootloader: %s\n", (char*)mbd->boot_loader_name);
}
