#include <stdio.h>
#include <stdlib.h>

[[noreturn]] void abort(void) {
#ifdef OSFS_LIBK
  // TODO: proper kernel panic
  printf("kernel: panic: abort()\n");
#else
  // TODO: terminate userspace program
  printf("abort()\n");
#endif
  while (1) {
  }
  __builtin_unreachable();
}
