#ifndef _KERNEL_HANDLERS_H
#define _KERNEL_HANDLERS_H

#include <stdint.h>
#include <sys/cdefs.h>

// For gate types/rings below
#include "interrupts.h" // IWYU pragma: keep

#define INTERRUPT __attribute((interrupt)) void

typedef struct PACKED interrupt_frame {
  uint32_t ip;
  segment_selector_t cs;
  uint16_t __reserved1;
  uint32_t flags;
} interrupt_frame_t;
_Static_assert(sizeof(interrupt_frame_t) == 12, "");

extern INTERRUPT breakpoint_interrupt_handler(interrupt_frame_t *frame);
extern INTERRUPT page_fault_interrupt_handler(interrupt_frame_t *frame,
                                              uint32_t error_code);
extern INTERRUPT double_fault_interrupt_handler(interrupt_frame_t *frame,
                                                uint32_t error_code);

extern INTERRUPT test_interrupt_handler(interrupt_frame_t *frame);

#define INTERRUPT_HANDLERS                                                     \
  X("breakpoint", 0x03, (uint32_t)breakpoint_interrupt_handler,                \
    IDT_GATE_TYPE_TRAP32, IDT_DPL_RING0)                                       \
  X("double fault", 0x08, (uint32_t)double_fault_interrupt_handler,            \
    IDT_GATE_TYPE_INTERRUPT32, IDT_DPL_RING0)                                  \
  X("page fault", 0x0E, (uint32_t)page_fault_interrupt_handler,                \
    IDT_GATE_TYPE_INTERRUPT32, IDT_DPL_RING0)                                  \
  X("test", 0x80, (uint32_t)test_interrupt_handler, IDT_GATE_TYPE_INTERRUPT32, \
    IDT_DPL_RING0)

#endif // _KERNEL_HANDLERS_H
