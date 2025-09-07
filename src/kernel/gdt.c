#include <string.h>

#include "gdt.h"

extern void gdt_flush(uint32_t);
extern void tss_flush();

#define GDT_DESCRIPTOR_COUNT 6

gdt_descriptor_t gdt_entries[GDT_DESCRIPTOR_COUNT] = {0};
tss_entry_t tss_entry = {0};
gdt_ptr_t gdt_ptr = {0};

void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
             uint8_t flags);
void write_tss(uint32_t num, uint16_t ss0, uint16_t esp0);

void gdt_init() {
  memset(gdt_entries, 0, sizeof(gdt_entries));

  gdt_ptr.limit = sizeof(gdt_entries) - 1;
  gdt_ptr.base = (uint32_t)&gdt_entries;

  // Set null segment
  set_gdt(0, 0, 0, 0, 0);
  // Set kernel code segment
  set_gdt(1, 0, 0xFFFFFFFF,
          GDT_ACCESS_RW | GDT_ACCESS_EXEC | GDT_ACCESS_CODE |
              GDT_ACCESS_PRESENT,
          GDT_FLAG_32BIT | GDT_FLAG_4KiB);
  // Set kernel data segment
  set_gdt(2, 0, 0xFFFFFFFF,
          GDT_ACCESS_RW | GDT_ACCESS_DATA | GDT_ACCESS_PRESENT,
          GDT_FLAG_32BIT | GDT_FLAG_4KiB);
  // Set user code segment
  set_gdt(3, 0, 0xFFFFFFFF,
          GDT_ACCESS_RW | GDT_ACCESS_EXEC | GDT_ACCESS_CODE |
              GDT_ACCESS_DPL_LO | GDT_ACCESS_PRESENT,
          GDT_FLAG_32BIT | GDT_FLAG_4KiB);
  // Set user data segment
  set_gdt(4, 0, 0xFFFFFFFF,
          GDT_ACCESS_RW | GDT_ACCESS_DATA | GDT_ACCESS_DPL_LO |
              GDT_ACCESS_PRESENT,
          GDT_FLAG_32BIT | GDT_FLAG_4KiB);
  // Set task state segment
  write_tss(5, 0x10, 0x0);

  // Flush GDT/TSS
  gdt_flush((uint32_t)&gdt_ptr);
  tss_flush();
}

void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
             uint8_t flags) {
  gdt_entries[num].base_lo = base & 0xFFFF;
  gdt_entries[num].base_mid = (base >> 16) & 0xFF;
  gdt_entries[num].base_hi = (base >> 24) & 0xFF;

  gdt_entries[num].limit_lo = limit & 0xFFFF;
  gdt_entries[num].limit_hi = (limit >> 16) & 0x0F;

  gdt_entries[num].access = access;
  gdt_entries[num].flags = flags & 0x0F;
}

void write_tss(uint32_t num, uint16_t ss0, uint16_t esp0) {
  uint32_t base = (uint32_t)&tss_entry;
  uint32_t limit = base + sizeof(tss_entry_t);

  memset(&tss_entry, 0, sizeof(tss_entry));
  tss_entry.ss0 = ss0;
  tss_entry.esp0 = esp0;
  tss_entry.cs = 0x08 | 0x3; // '| 0x3' allows kernel mode switch
  tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs =
      0x10 | 0x3; // Same as above

  set_gdt(num, base, limit,
          GDT_ACCESS_ACCESSED | GDT_ACCESS_EXEC | GDT_ACCESS_DPL_LO |
              GDT_ACCESS_PRESENT,
          0x00);
}
