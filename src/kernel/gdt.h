#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stdint.h>

enum gdt_access {
  GDT_ACCESS_ACCESSED = 1 << 0,
  GDT_ACCESS_RW = 1 << 1,
  GDT_ACCESS_DIR_CONFORM = 1 << 2,
  GDT_ACCESS_EXEC = 1 << 3,
  GDT_ACCESS_CODE = 1 << 4,
  GDT_ACCESS_DATA = 1 << 4,
  GDT_ACCESS_DPL_HI = 0b00 << 5, // default privilege
  GDT_ACCESS_DPL_MED = 0b01 << 5,
  GDT_ACCESS_DPL_LO = 0b11 << 5,
  GDT_ACCESS_PRESENT = 1 << 7,
};

enum gdt_flag {
  GDT_FLAG_RESERVED = 1 << 0, // reserved
  GDT_FLAG_LONG_MODE = 1 << 1,
  GDT_FLAG_32BIT = 1 << 2,
  GDT_FLAG_4KiB = 1 << 3,
};

typedef struct gdt_entry {
  uint16_t limit_lo;
  uint16_t base_lo;
  uint8_t base_mid;
  uint8_t access;
  uint8_t limit_hi : 4;
  uint8_t flags : 4;
  uint8_t base_hi;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

void gdt_init();

// void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
//              uint8_t flags);

#endif // _KERNEL_GDT_H
