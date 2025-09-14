#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64

#define KEYBOARD_STATUS_OUTPUT_FULL (1 << 0)
#define KEYBOARD_STATUS_INPUT_FULL (1 << 1)

#define KEYBOARD_SCANCODE_RELEASED (1 << 7)

#define KEYBOARD_BUFFER_SIZE 256

enum keyboard_flags {
  KEY_FLAG_NONE = 0,
  KEY_FLAG_CTRL = 1 << 0,
  KEY_FLAG_SHIFT = 1 << 1,
  KEY_FLAG_ALT = 1 << 2,
  KEY_FLAG_CAPS = 1 << 3,
};

void keyboard_init(void);
int keyboard_poll_ascii(char *ch);

// Essentially only for use by the interrupt handler
void keyboard_push_scancode(uint8_t scancode);

#endif // _KERNEL_KEYBOARD_H
