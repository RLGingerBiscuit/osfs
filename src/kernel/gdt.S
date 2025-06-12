.global gdt_flush
.type gdt_flush, @function
gdt_flush:
        // Disable interrupts
        cli

        // Set gdt ptr
        movl 4(%esp), %eax 
        lgdt (%eax)

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

.global tss_flush
.type tss_flush, @function
tss_flush:
        mov $0x2B, %ax
        ltr %ax
        ret
