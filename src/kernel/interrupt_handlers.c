#include <stdio.h>

#include "interrupt_handlers.h"

__attribute__((interrupt)) void
test_interrupt_handler(interrupt_frame_t *frame) {
  printf("Hello from test interrupt!\n");
  printf("IP: 0x%lx, CS: 0x%hx, Flags: 0x%lx\n\n", (unsigned long)frame->ip,
         *((uint16_t *)&frame->cs), (unsigned long)frame->flags);
}
