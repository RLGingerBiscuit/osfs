#include <stddef.h>
#include <sys/cdefs.h>

#include "interrupt_handlers.h"
#include "interrupts.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"

#include <stdio.h>

#define SET_FLAG_COND(x, y, c) (x = c ? x | y : x & ~y)

// UK QWERTY scancode to ASCII mapping (scancode set 1)
// Only printable chars + some control chars
static const char en_gb_scancode_to_ascii[] = {
    0,   0,   '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '#', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static const char en_gb_scancode_to_ascii_shift[] = {
    0,   0,   '!',  '"',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '@',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};

_Static_assert(sizeof(en_gb_scancode_to_ascii) ==
                   sizeof(en_gb_scancode_to_ascii_shift),
               "");

static struct {
  char buf[KEYBOARD_BUFFER_SIZE]; // TODO: keep raw scancodes until input
                                  // requested?
  size_t read;
  size_t count;
  enum keyboard_flags flags;
} g_keyboard_state;

void keyboard_init(void) {
  g_keyboard_state.read = g_keyboard_state.count = 0;
  g_keyboard_state.flags = KEY_FLAG_NONE;

  // Tell PIC to send us keyboard interrupts
  pic_clear_mask(IRQ_KEYBOARD);

  // Tell the interrupt table about our handler
  idt_set_interrupt_handler(PIC_OFFSET + IRQ_KEYBOARD,
                            keyboard_interrupt_handler);

  // TODO: more configuration?

  // Clear pending keyboard data
  while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_OUTPUT_FULL) {
    inb(KEYBOARD_DATA_PORT);
  }
}

void keyboard_push_scancode(uint8_t scancode) {
  int key_released = (scancode & KEYBOARD_SCANCODE_RELEASED) != 0;
  scancode &= 0x7f;

  // Handle special keys
  switch (scancode) {
  case 0x1d: // Left/Right Control
             // NOTE: RCTRL is E0 1D
    SET_FLAG_COND(g_keyboard_state.flags, KEY_FLAG_CTRL, !key_released);
    return;
  case 0x2a: // Left Shift
  case 0x36: // Right Shift
    SET_FLAG_COND(g_keyboard_state.flags, KEY_FLAG_SHIFT, !key_released);
    return;
  case 0x38: // Left Alt
    // TODO: extended scancodes (for RALT, E0 11)
    SET_FLAG_COND(g_keyboard_state.flags, KEY_FLAG_ALT, !key_released);
    return;
  case 0x3a: // Caps Lock
    if (!key_released)
      SET_FLAG_COND(g_keyboard_state.flags, KEY_FLAG_CAPS,
                    !(g_keyboard_state.flags & KEY_FLAG_CAPS));
    return;

  case 0xe0: // Extended scan code
             // TODO: extended scancodes
    return;
  }

  // Ignore key releases (for now)
  if (key_released)
    return;

  // TODO: some way of keeping track of these
  if ((g_keyboard_state.flags & (KEY_FLAG_ALT | KEY_FLAG_CTRL)) != 0)
    return;

  char ascii = 0;
  if (scancode < sizeof(en_gb_scancode_to_ascii)) {
    if (g_keyboard_state.flags & KEY_FLAG_SHIFT)
      ascii = en_gb_scancode_to_ascii_shift[scancode];
    else
      ascii = en_gb_scancode_to_ascii[scancode];

    // Apply caps lock for letters
    if ((g_keyboard_state.flags & KEY_FLAG_CAPS) && ascii >= 'a' &&
        ascii <= 'z') {
      ascii = ascii - 'a' + 'A';
    } else if ((g_keyboard_state.flags & KEY_FLAG_CAPS) && ascii >= 'A' &&
               ascii <= 'Z') {
      ascii = ascii - 'A' + 'a';
    }
  }

  if (ascii > 0 && g_keyboard_state.count < KEYBOARD_BUFFER_SIZE) {
    size_t write =
        (g_keyboard_state.read + g_keyboard_state.count) % KEYBOARD_BUFFER_SIZE;
    g_keyboard_state.buf[write] = ascii;
    g_keyboard_state.count += 1;
  }
}

int keyboard_poll_ascii(char *ch) {
  if (g_keyboard_state.count == 0)
    return 0;

  char c = g_keyboard_state.buf[g_keyboard_state.read];
  g_keyboard_state.read = (g_keyboard_state.read) % KEYBOARD_BUFFER_SIZE;
  g_keyboard_state.count--;

  *ch = c;

  return 1;
}
