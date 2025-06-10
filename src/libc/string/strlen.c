#include <string.h>

size_t strlen(const char *restrict str) {
  const char *s = str;
  const char *e = str;
  for (; *e != 0; e++) {
  }
  return ((size_t)e - (size_t)s);
}
