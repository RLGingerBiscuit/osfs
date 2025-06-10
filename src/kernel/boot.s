# kernel_main is where our C code starts
.extern kernel_main

# Some stupid numbers
.set MB_ALIGN, 1 << 0
.set MB_MEMINFO, 1 << 1
.set MB_MAGIC, 0x1BADB002
.set MB_FLAGS, MB_ALIGN | MB_MEMINFO
.set MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS)

# Multiboot so we can actually run this thang
.section .multiboot
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM

# Literally just the stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16KB stack'll do for now
stack_top:

.section .text

# Linker needs to know where to start
.global _start
_start:
        # Set up the stack
        mov $stack_top, %esp

        # Multiboot magic
        push %eax
        # Multiboot info
        push %ebx

        # Run the kernel
        call kernel_main

        # We should never get here, but IF we do let's do something about it
hang:
        cli # Disable interrupts
        hlt # Halt
        jmp hang # If all else fails, busy loop

.size _start, . - _start
