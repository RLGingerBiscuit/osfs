#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdint.h>

typedef uint16_t port_t;

static inline void outb(port_t port, uint8_t val) {
  __asm__ volatile("outb %b0, %w1" ::"a"(val), "Nd"(port) : "memory");
}
static inline void outw(port_t port, uint16_t val) {
  __asm__ volatile("outw %w0, %w1" ::"a"(val), "Nd"(port) : "memory");
}
static inline void outd(port_t port, uint32_t val) {
  __asm__ volatile("outl %d0, %w1" ::"a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(port_t port) {
  uint8_t val;
  __asm__ volatile("inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}
static inline uint16_t inw(port_t port) {
  uint16_t val;
  __asm__ volatile("inw %w1, %w0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}
static inline uint32_t ind(port_t port) {
  uint32_t val;
  __asm__ volatile("inl %w1, %d0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void wait(void) { outb(0x80, 0); }

#endif // _KERNEL_IO_H
