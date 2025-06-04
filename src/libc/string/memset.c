#include <string.h>

void *memset(void *dest, int ch, size_t count) {
  unsigned char *d = (unsigned char *)dest;
  for (size_t i = 0; i < count; ++i) {
    d[i] = (char)ch;
  }
  return d;
}
