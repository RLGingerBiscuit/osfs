#include <string.h>

void *memcpy(void *dest, const void *src, size_t count) {
  unsigned char *s = (unsigned char *)src;
  unsigned char *d = (unsigned char *)dest;
  for (size_t i = 0; i < count; ++i) {
    d[i] = s[i];
  }
  return d;
}
