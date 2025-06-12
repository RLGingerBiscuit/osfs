#ifndef _CDEFS_H
#define _CDEFS_H

// Taken straight from
// https://patchwork.ozlabs.org/project/glibc/patch/54D40347.3020508@cs.ucla.edu/#962425
#define _Static_assert(expr, mesg)                                              \
  extern int (*__static_assert__(                                              \
      void))[!!sizeof(struct { int __assertion_failed__ : (expr) ? 2 : -1; })]

#endif // _CDEFS_H
