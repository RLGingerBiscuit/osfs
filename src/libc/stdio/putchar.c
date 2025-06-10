#include <stdio.h>

#ifdef OSFS_LIBK
#include <kernel/tty.h>
#endif

int putchar(int ch) {
#ifdef OSFS_LIBK
  char c = (char)ch;
  tty_putc(c);
#else
  // TODO: userspace putchar
#endif
  return ch;
}
