#include <kernel/kstdio.h>

int vga_printf(const char *restrict fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vga_vprintf(fmt, ap);
  va_end(ap);
  return ret;
}
