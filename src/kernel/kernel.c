#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>

#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "pic.h"

multiboot_info_t *multiboot_info_ptr;
extern unsigned char _kernel_start[], _kernel_end[];
extern unsigned char _kernel_readonly_start[], _kernel_readonly_end[];
extern unsigned char _kernel_readwrite_start[], _kernel_readwrite_end[];

void kernel_main() {
  tty_init();
  gdt_init();
  idt_init();

  pic_init();
  keyboard_init();

  printf("Hello %s World\n", "Kernel");
  printf("The number is %d\n", 42);
  printf("Multiboot: %p\n", multiboot_info_ptr);
  printf("Bootloader: %s\n", (char *)multiboot_info_ptr->boot_loader_name);
  printf("Kernel Start:\t\t%p\tKernel End:\t\t%p\n", _kernel_start,
         _kernel_end);
  printf("Kernel RO Start:\t%p\tKernel RO End:\t%p\n", _kernel_readonly_start,
         _kernel_readonly_end);
  printf("Kernel RW Start:\t%p\tKernel RW End:\t%p\n", _kernel_readwrite_start,
         _kernel_readwrite_end);
  printf("\n");

  while (1) {
    char ch;
    if (keyboard_poll_ascii(&ch)) {
      switch (ch) {
      case 8: // Backspace
        tty_modpos(-1, 0);
        tty_putc(' ');
        tty_modpos(-1, 0);
        break;
      default:
        tty_putc(ch);
        break;
      }

      // Sleep until next interrupt
      __asm__("hlt");
    }
  }
}
