#include <kernel/panic.h>
#include <kernel/vga.h>

[[noreturn]] void trap() {
  while (1) {
  }
}

[[noreturn]] void panic(const char *str) {
  vga_printstr("KERNEL PANIC: ");
  vga_printstr(str);
  vga_putc('\n');
  trap();
}
