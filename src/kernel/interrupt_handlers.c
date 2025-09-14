#include <stdio.h>

#include "interrupt_handlers.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"

#define PRINT_STACK_FRAME()                                                    \
  printf("IP: 0x%lx, CS: 0x%hx, Flags: 0x%lx\n\n", (unsigned long)frame->ip,   \
         *((uint16_t *)&frame->cs), (unsigned long)frame->flags);

#define STUB_HANDLER(name)                                                     \
  INTERRUPT name##_interrupt_handler(interrupt_frame_t *frame) {               \
    printf("Hello from " #name " interrupt!\n");                               \
    PRINT_STACK_FRAME();                                                       \
  }

INTERRUPT breakpoint_interrupt_handler(interrupt_frame_t *frame) {
  printf("\nEXCEPTION: BREAKPOINT\n");
  PRINT_STACK_FRAME();
}

INTERRUPT page_fault_interrupt_handler(interrupt_frame_t *frame,
                                       uint32_t error_code) {
  uintptr_t accessed_address = 0;

  __asm__ volatile("movl %%cr2, %d0" : "=a"(accessed_address) : : "memory");

  printf("\nEXCEPTION: PAGE FAULT\n");
  printf("Accessed Address: 0x%lx\n", (unsigned long)accessed_address);
  printf("Error Code: 0x%lx\n", (unsigned long)error_code);
  PRINT_STACK_FRAME();

  while (1) {
    __asm__("hlt");
  }
}

INTERRUPT double_fault_interrupt_handler(interrupt_frame_t *frame,
                                         uint32_t error_code) {
  (void)error_code;

  printf("\nEXCEPTION: DOUBLE FAULT\n");
  PRINT_STACK_FRAME();

  while (1) {
    __asm__("hlt");
  }
}

STUB_HANDLER(test);

INTERRUPT keyboard_interrupt_handler(interrupt_frame_t *frame) {
  (void)frame;

  uint8_t scancode = inb(KEYBOARD_DATA_PORT);

  keyboard_push_scancode(scancode);

  pic_eoi(IRQ_KEYBOARD);
}
