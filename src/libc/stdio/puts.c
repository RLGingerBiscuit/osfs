#include <stdio.h>
#include <string.h>

#include <kernel/tty.h>

int puts(const char *str) {
  size_t len = strlen(str);
  tty_print(str, len);
  return (int)len;
}
