#ifndef _KERNEL_HANDLERS_H
#define _KERNEL_HANDLERS_H

#include <stdint.h>
#include <sys/cdefs.h>

// For gate types/rings below
#include "interrupts.h" // IWYU pragma: keep

typedef struct PACKED interrupt_frame {
  uint32_t ip;
  segment_selector_t cs;
  uint16_t __reserved1;
  uint32_t flags;
} interrupt_frame_t;
_Static_assert(sizeof(interrupt_frame_t) == 12, "");

extern __attribute__((interrupt)) void
test_interrupt_handler(interrupt_frame_t *frame);

#define INTERRUPT_HANDLERS                                                     \
  X("test", 0x80, (uint32_t)test_interrupt_handler, IDT_GATE_TYPE_INTERRUPT32, \
    IDT_DPL_RING0)

#endif // _KERNEL_HANDLERS_H
