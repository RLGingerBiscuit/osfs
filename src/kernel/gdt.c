#include "gdt.h"

extern void gdt_flush(uint32_t);

gdt_entry_t gdt_entries[5] = {0};
gdt_ptr_t gdt_ptr = {0};

void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
             uint8_t flags);

void gdt_init() {
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

  // Flush GDT
  gdt_flush((uint32_t)&gdt_ptr);
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
