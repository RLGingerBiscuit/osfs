.global idt_flush
.type idt_flush, @function
idt_flush:
        // Disable interrupts
        cli

        // Set idt ptr
        movl 4(%esp), %eax 
        lidt (%eax)

        // Reload segment registers
        mov $0x10, %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        mov %ax, %ds
        jmp $0x08, $.flush

.flush:
        // Enable interrupts
        sti
        ret
