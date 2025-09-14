#include "pic.h"
#include "io.h"

#define PIC_CMD_EOI 0x20

#define PIC_ICW1_ICW4 0x01
#define PIC_ICW1_SINGLE 0x02
#define PIC_ICW1_INTERVAL4 0x04
#define PIC_ICW1_LEVEL 0x08
#define PIC_ICW1_INIT 0x10

#define PIC_ICW4_8086 0x01
#define PIC_ICW4_AUTO_EOI 0x02
#define PIC_ICW4_BUF_SLAVE 0x08
#define PIC_ICW4_BUF_MASTER 0x0c
#define PIC_ICW4_SFNM 0x10

void pic_init(void) {
  // ICW1: Start the sequence for both PICs
  outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  wait();
  outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  wait();

  // ICW2: Set offsets
  outb(PIC1_DATA, PIC1_OFFSET);
  wait();
  outb(PIC2_DATA, PIC2_OFFSET);
  wait();

  // ICW3: Cascate configuration
  outb(PIC1_DATA, 1 << 2); // Tell (Master) PIC 1 that there's a slave at 2
  wait();
  outb(PIC2_DATA, 2); // Tell (Slave) PIC 2 that it's 2
  wait();

  // ICW4: Mode configuration
  outb(PIC1_DATA, PIC_ICW4_8086);
  wait();
  outb(PIC2_DATA, PIC_ICW4_8086);
  wait();

  // Disable everything (for now)
  outb(PIC1_DATA, 0xff);
  outb(PIC2_DATA, 0xff);
}

void pic_clear_mask(uint8_t irq) {
  uint16_t port = PIC1_DATA;
  if (irq >= 8) {
    port = PIC2_DATA;
    irq -= 8;
  }
  uint8_t val = inb(port) & ~(1 << irq);
  outb(port, val);
}

void pic_set_mask(uint8_t irq) {
  uint16_t port = PIC1_DATA;
  if (irq >= 8) {
    port = PIC2_DATA;
    irq -= 8;
  }
  uint8_t val = inb(port) | (1 << irq);
  outb(port, val);
}

void pic_eoi(uint8_t irq) {
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_CMD_EOI);
  outb(PIC1_COMMAND, PIC_CMD_EOI);
}
