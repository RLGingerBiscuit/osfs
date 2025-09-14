#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

#include <stdint.h>

#define PIC_OFFSET 0x20
#define PIC1_OFFSET PIC_OFFSET
#define PIC2_OFFSET (PIC1_OFFSET + 8)

#define PIC1_COMMAND 0x20
#define PIC1_DATA (PIC1_COMMAND + 1)

#define PIC2_COMMAND 0xA0
#define PIC2_DATA (PIC2_COMMAND + 1)

#define IRQ_KEYBOARD 1

void pic_init(void);
void pic_clear_mask(uint8_t irq);
void pic_set_mask(uint8_t irq);
void pic_eoi(uint8_t irq);

#endif // _KERNEL_PIC_H
