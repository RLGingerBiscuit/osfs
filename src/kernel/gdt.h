#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stdint.h>
#include <sys/cdefs.h>

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

  GDT_ACCESS_SYSTEM_16BIT = 1 << 0,
  GDT_ACCESS_SYSTEM_32BIT = 1 << 3,
  GDT_ACCESS_SYSTEM_LDT = 1 << 1,
  GDT_ACCESS_SYSTEM_BUSY = 1 << 1,
};

enum gdt_flag {
  GDT_FLAG_RESERVED = 1 << 0, // reserved
  GDT_FLAG_LONG_MODE = 1 << 1,
  GDT_FLAG_32BIT = 1 << 2,
  GDT_FLAG_4KiB = 1 << 3,
};

typedef struct PACKED gdt_descriptor {
  uint16_t limit_lo;
  uint16_t base_lo;
  uint8_t base_mid;
  uint8_t access;
  uint8_t limit_hi : 4;
  uint8_t flags : 4;
  uint8_t base_hi;
} gdt_descriptor_t;
_Static_assert(sizeof(gdt_descriptor_t) == 8, "");

typedef struct PACKED gdt_ptr {
  uint16_t limit;
  uint32_t base;
} gdt_ptr_t;
_Static_assert(sizeof(gdt_ptr_t) == 6, "");

typedef struct PACKED tss_entry {
  uint32_t prev_tss;
  uint32_t esp0;
  uint32_t ss0;
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldtr;
  uint32_t iopb;
  uint32_t ssp;
} tss_entry_t;
_Static_assert(sizeof(tss_entry_t) == 0x6c, "");

void gdt_init(void);

// void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
//              uint8_t flags);

// void write_tss(uint32_t num, uint16_t ss0, uint16_t esp0);

#endif // _KERNEL_GDT_H
