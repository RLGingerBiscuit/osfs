#include <stdio.h>
#include <string.h>

#include "interrupts.h"

typedef struct interrupt_frame {
  uint32_t flags;
  uint32_t cs;
  uint32_t ip;
} interrupt_frame_t;

extern void idt_flush(uint32_t);
extern void isr_wrapper(void);

#define IDT_DESCRIPTOR_COUNT 256

idt_descriptor_t idt_entries[IDT_DESCRIPTOR_COUNT] = {0};
idt_ptr_t idt_ptr = {0};

void set_idt(uint32_t num, uint32_t offset, segment_selector_t selector,
             uint8_t gate_type, uint8_t dpl, uint8_t present);

void idt_init(void) {
  memset(idt_entries, 0, sizeof(idt_entries));

  idt_ptr.size = sizeof(idt_entries) - 1;
  idt_ptr.offset = (uint32_t)&idt_entries;

  set_idt(0x80, (uint32_t)isr_wrapper, (segment_selector_t){.index = 0x08 >> 3},
          IDT_GATE_TYPE_INTERRUPT32, IDT_DPL_RING0, 1);

  // Flush IDT
  idt_flush((uint32_t)&idt_ptr);

  __asm__("int $0x80");
}

void set_idt(uint32_t num, uint32_t offset, segment_selector_t selector,
             uint8_t gate_type, uint8_t dpl, uint8_t present) {
  idt_entries[num].offset_lo = offset & 0xffff;
  idt_entries[num].offset_hi = (offset >> 16) & 0xffff;

  idt_entries[num].selector = selector;
  idt_entries[num].gate_type = gate_type;
  idt_entries[num].dpl = dpl;
  idt_entries[num].present = present;
}

void interrupt_handler(interrupt_frame_t frame) {
  printf("Hello from interrupt!\n");
  printf("IP: 0x%lx, CS: 0x%lx, Flags: 0x%lx\n\n", frame.ip, frame.cs,
         frame.flags);
}
