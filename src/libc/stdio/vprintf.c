
#include <limits.h>
#include <stdio.h>

#include <kernel/panic.h>
#include <kernel/vga.h>

typedef enum arg_type {
  // Prefixes
  BARE = 0, // int
  HHPRE,    // char
  HPRE,     // short
  LPRE,     // long
  LLPRE,    // long long
  ZPRE,     // size
  JPRE,     // intmax
  TPRE,     // ptrdiff
  BIGLPRE,  // long double
  // Types
  CHAR,
  UCHAR,
  SHORT,
  USHORT,
  INT,
  UINT,
  LONG,
  ULONG,
  LLONG,
  ULLONG,
  SIZET,
  IMAX,
  UMAX,
  PTR,
  UIPTR,
  PTRDIFF,
  FLOAT,
  DOUBLE,
  NOARG,
} arg_type;

typedef union arg {
  uintmax_t i;
  long double d;
  uintptr_t p;
} arg;

enum {
  PNONE,
  PNEG = 1 << 0,
  PHEX = 1 << 1,
};

#define LOWERCASE_BIT 32

#define S(x) [(x) - 'A']

static const char states[]['z' - 'A' + 1] = {
    {
        // BARE
        S('d') = INT,   S('i') = INT,   S('o') = UINT,  S('u') = UINT,
        S('x') = UINT,  S('X') = UINT,  S('e') = FLOAT, S('f') = FLOAT,
        S('g') = FLOAT, S('a') = FLOAT, S('E') = FLOAT, S('F') = FLOAT,
        S('G') = FLOAT, S('A') = FLOAT, S('c') = INT,   S('C') = UINT,
        S('s') = PTR,   S('S') = PTR,   S('p') = UIPTR, S('n') = PTR,
        S('m') = NOARG, S('l') = LPRE,  S('h') = HPRE,  S('L') = BIGLPRE,
        S('z') = ZPRE,  S('j') = JPRE,  S('t') = TPRE,
    },
    {
        // HHPRE
        S('d') = CHAR,
        S('i') = CHAR,
        S('o') = UCHAR,
        S('u') = UCHAR,
        S('x') = UCHAR,
        S('X') = UCHAR,
        S('n') = PTR,
    },
    {
        // HPRE
        S('d') = SHORT,
        S('i') = SHORT,
        S('o') = USHORT,
        S('u') = USHORT,
        S('x') = USHORT,
        S('X') = USHORT,
        S('n') = PTR,
        S('h') = HHPRE,
    },
    {
        // LPRE
        S('d') = LONG,
        S('i') = LONG,
        S('o') = ULONG,
        S('u') = ULONG,
        S('x') = ULONG,
        S('X') = ULONG,
        S('e') = FLOAT,
        S('f') = FLOAT,
        S('g') = FLOAT,
        S('a') = FLOAT,
        S('E') = FLOAT,
        S('F') = FLOAT,
        S('G') = FLOAT,
        S('A') = FLOAT,
        S('c') = UINT,
        S('s') = PTR,
        S('n') = PTR,
        S('l') = LLPRE,
    },
    {
        // LLPRE
        S('d') = LLONG,
        S('i') = LLONG,
        S('o') = ULLONG,
        S('u') = ULLONG,
        S('x') = ULLONG,
        S('X') = ULLONG,
        S('n') = PTR,
    },
    {
        // ZPRE
        S('d') = PTRDIFF,
        S('i') = PTRDIFF,
        S('o') = SIZET,
        S('u') = SIZET,
        S('x') = SIZET,
        S('X') = SIZET,
        S('n') = PTR,
    },
    {
        // JPRE
        S('d') = IMAX,
        S('i') = IMAX,
        S('o') = UMAX,
        S('u') = UMAX,
        S('x') = UMAX,
        S('X') = UMAX,
        S('n') = PTR,
    },
    {
        // TPRE
        S('d') = PTRDIFF,
        S('i') = PTRDIFF,
        S('o') = SIZET,
        S('u') = SIZET,
        S('x') = SIZET,
        S('X') = SIZET,
        S('n') = PTR,
    },
    {
        // BIGLPRE
        S('e') = DOUBLE,
        S('f') = DOUBLE,
        S('g') = DOUBLE,
        S('a') = DOUBLE,
        S('E') = DOUBLE,
        S('F') = DOUBLE,
        S('G') = DOUBLE,
        S('A') = DOUBLE,
        S('n') = PTR,
    },
};

static void get_arg(union arg *arg, enum arg_type type, va_list *ap);

static char *fmt_dec(uintmax_t x, char *te);
static char *fmt_hex(uintmax_t x, char *te, char lower);
static char *fmt_oct(uintmax_t x, char *te);

int vprintf(const char *restrict fmt, va_list ap) {
  char buf[sizeof(uintmax_t) * 3];
  char *ts, *te;
  char ch;

  int written = 0;
  size_t len = 0;

  arg arg = {0};
  arg_type type = BARE;
  arg_type pre_type = BARE;
  unsigned char prefix = PNONE;

  while (1) {
    written += len;
    if (*fmt == 0)
      break;

    for (ts = (char *)fmt; *ts && *ts != '%'; ts++) {
    }
    if (fmt != ts) {
      len = (ts - fmt);
      vga_print(fmt, len);
      fmt = ts;
      continue;
    }

    // skip '%'
    fmt++;

    // write literal '%'s
    if (*fmt == '%') {
      fmt++;
      vga_putc('%');
      continue;
    }

    ch = *fmt;

    // run the state machine
    type = BARE;
    do {
      pre_type = type;
      type = states[type] S(*fmt++);
    } while (type <= BIGLPRE);

    if (type == 0)
      goto inval;
    if (type == NOARG)
      goto inval;

    get_arg(&arg, type, &ap);

    te = buf + sizeof(buf);
    len = 0;
    prefix = 0;

    switch (ch) {
    case 'd':
    case 'i':
      if (arg.i > INT_MAX) {
        prefix |= PNEG;
        arg.i = -arg.i;
      }
    case 'u':
      ts = fmt_dec(arg.i, te);
      break;

    case 'p':
      prefix |= PHEX;
    case 'x':
    case 'X':
      ts = fmt_hex(arg.i, te, ch & LOWERCASE_BIT);
      break;

    case 'o':
      ts = fmt_oct(arg.i, te);
      break;
    }

    if (prefix & PHEX && arg.i != 0) {
      *--ts = 'X' | (ch & LOWERCASE_BIT);
      *--ts = '0';
    }

    if (prefix & PNEG) {
      *--ts = '-';
    }

    len = (te - ts);
    vga_print(ts, len);
  }

  (void)arg;
  (void)pre_type;

  return written;

inval:
  return -1;
}

static void get_arg(union arg *arg, enum arg_type type, va_list *ap) {
  switch (type) {
  case CHAR:
    arg->i = (signed char)va_arg(*ap, int);
    break;
  case UCHAR:
    arg->i = (unsigned char)va_arg(*ap, int);
    break;
  case SHORT:
    arg->i = (short)va_arg(*ap, int);
    break;
  case USHORT:
    arg->i = (unsigned short)va_arg(*ap, int);
    break;
  case INT:
    arg->i = va_arg(*ap, int);
    break;
  case UINT:
    arg->i = va_arg(*ap, unsigned int);
    break;
  case LONG:
    arg->i = va_arg(*ap, long);
    break;
  case ULONG:
    arg->i = va_arg(*ap, unsigned long);
    break;
  case LLONG:
    arg->i = va_arg(*ap, long long);
    break;
  case ULLONG:
    arg->i = va_arg(*ap, unsigned long long);
    break;
  case SIZET:
    arg->i = va_arg(*ap, size_t);
    break;
  case IMAX:
    arg->i = va_arg(*ap, intmax_t);
    break;
  case UMAX:
    arg->i = va_arg(*ap, uintmax_t);
    break;
  case PTR:
    arg->i = va_arg(*ap, intptr_t);
    break;
  case PTRDIFF:
    arg->i = va_arg(*ap, ptrdiff_t);
    break;
  case UIPTR:
    arg->i = va_arg(*ap, uintptr_t);
    break;
  case FLOAT:
    arg->i = va_arg(*ap, double);
    break;
  case DOUBLE:
    arg->i = va_arg(*ap, long double);
    break;
  case BARE:
  case HHPRE:
  case HPRE:
  case LPRE:
  case LLPRE:
  case ZPRE:
  case JPRE:
  case TPRE:
  case BIGLPRE:
  case NOARG:
    panic("printf: Attempted to get argument of util type");
  }
}

static char *fmt_dec(uintmax_t x, char *te) {
  for (; x >= 10; x /= 10)
    *--te = '0' + (x % 10);
  if (x > 0)
    *--te = '0' + x;
  return te;
}

static char *fmt_hex(uintmax_t x, char *te, char lower) {
  static const char *hexdig = "0123456789ABCDEF";
  for (; x > 0; x >>= 4)
    *--te = hexdig[x & 0xf] | lower;
  return te;
}

static char *fmt_oct(uintmax_t x, char *te) {
  for (; x > 0; x >>= 3)
    *--te = '0' + (x & 0x7);
  return te;
}
