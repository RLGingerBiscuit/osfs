#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include "sys/cdefs.h"

#include <stdint.h>

#define INTERRUPT __attribute((interrupt)) void

enum idt_dpl {
  IDT_DPL_RING0 = 0x00,
  IDT_DPL_RING1 = 0x01,
  IDT_DPL_RING2 = 0x02,
  IDT_DPL_RING3 = 0x03
};

enum idt_gate_type {
  IDT_GATE_TYPE_TASK = 0b0101,
  IDT_GATE_TYPE_INTERRUPT16 = 0b0110,
  IDT_GATE_TYPE_TRAP16 = 0b0111,
  IDT_GATE_TYPE_INTERRUPT32 = 0b1110,
  IDT_GATE_TYPE_TRAP32 = 0b1111
};

// TODO: This is actually for GDTs and LDTs
typedef struct PACKED segment_selector {
  uint8_t rpl : 2;
  uint8_t tl : 1;
  uint16_t index : 13; // NOTE: needs to be >> 3
} segment_selector_t;
_Static_assert(sizeof(segment_selector_t) == 2, "");

typedef struct PACKED idt_descriptor {
  uint16_t offset_lo;
  segment_selector_t selector;
  uint8_t __reserved1;
  uint8_t gate_type : 4;
  uint8_t __reserved2 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_hi;
} idt_descriptor_t;
_Static_assert(sizeof(idt_descriptor_t) == 8, "");

typedef struct PACKED idt_ptr {
  uint16_t size;
  uint32_t offset;
} idt_ptr_t;
_Static_assert(sizeof(idt_ptr_t) == 6, "");

typedef struct PACKED interrupt_frame {
  uint32_t ip;
  segment_selector_t cs;
  uint16_t __reserved1;
  uint32_t flags;
} interrupt_frame_t;
_Static_assert(sizeof(interrupt_frame_t) == 12, "");

void idt_init(void);

// A simplified version of set_idt for outside use.
void idt_set_interrupt_handler(uint8_t num,
                               INTERRUPT (*handler)(interrupt_frame_t *frame));

#endif // _KERNEL_IDT_H
