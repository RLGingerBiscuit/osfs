#include <string.h>

#include "interrupt_handlers.h"
#include "interrupts.h"

extern void idt_flush(uint32_t);

#define IDT_DESCRIPTOR_COUNT 256

idt_descriptor_t idt_entries[IDT_DESCRIPTOR_COUNT] = {0};
idt_ptr_t idt_ptr = {0};

void set_idt(uint32_t num, uint32_t offset, segment_selector_t selector,
             enum idt_gate_type gate_type, enum idt_dpl dpl, uint8_t present);

void idt_init(void) {
  memset(idt_entries, 0, sizeof(idt_entries));

  idt_ptr.size = sizeof(idt_entries) - 1;
  idt_ptr.offset = (uint32_t)&idt_entries;

#define X(_name, num, ptr, gate_type, dpl)                                     \
  set_idt(num, ptr, (segment_selector_t){.index = 0x08 >> 3}, gate_type, dpl,  \
          1);
  INTERRUPT_HANDLERS
#undef X

  // Flush IDT
  idt_flush((uint32_t)&idt_ptr);

  __asm__("int $0x80");
}

void set_idt(uint32_t num, uint32_t offset, segment_selector_t selector,
             enum idt_gate_type gate_type, enum idt_dpl dpl, uint8_t present) {
  idt_entries[num].offset_lo = offset & 0xffff;
  idt_entries[num].offset_hi = (offset >> 16) & 0xffff;

  idt_entries[num].selector = selector;
  idt_entries[num].gate_type = gate_type;
  idt_entries[num].dpl = dpl;
  idt_entries[num].present = present;
}
