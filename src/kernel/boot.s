# kernel_main is where our C code starts
.extern kernel_main

# Kernel constants
.set KERNEL_VIRT_BASE, 0xC0000000

# Multiboot constants
.set MB_ALIGN, 1 << 0
.set MB_MEMINFO, 1 << 1
.set MB_MAGIC, 0x1BADB002
.set MB_FLAGS, MB_ALIGN | MB_MEMINFO
.set MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS)

# Multiboot so we can actually run this thang
.section .multiboot, "a", @progbits
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM

# Address fields
.long 0 # Header address
.long 0 # Load address
.long 0 # Load end address
.long 0 # BSS end address
.long 0 # Entry address

# Graphics fields
.long   0 # Mode (0=linear)
.long 800 # Width
.long 600 # Height
.long  32 # Depth


# Literally just the stack
.section .bss, "aw", @nobits
.align 16
stack_bottom:
.skip 16384 # 16KB stack'll do for now
stack_top:


# Boot page table/dir
.section .page_tables, "aw", @progbits
.align 4096
.global boot_page_directory
boot_page_directory:
.long 0b10000011 # Identity map first 4MiB as RW
.fill 768-1, 4, 0
.long 0<<22 | 0b10000011 # Map 0xC0000000 -> 0x0
.long 1<<22 | 0b10000011
.long 2<<22 | 0b10000011
.long 3<<22 | 0b10000011
.fill 256-4, 4, 0
# NOTE: Not needed as we're using 4MiB pages (please I just wanna get something working)
# .global boot_page_table
# boot_page_table:
# .skip 4096


# Bootloader/paging setup
.section .boot, "a", @progbits
.global _start
.type _start, @function
_start:
        # Tell CPU about our page directory (in physical space)
        movl $(boot_page_directory - KERNEL_VIRT_BASE), %ecx
        movl %ecx, %cr3

        # Enable 4MiB pages (PSE)
        movl %cr4, %eax
        orl $0x10, %eax
        movl %eax, %cr4

        # Enable paging and write-protect
        movl %cr0, %eax
        orl $0x80010000, %eax
        movl %eax, %cr0

        # Jump up to our kernel bootstrap
        lea higher_half, %eax
        jmp *%eax

.size _start, . - _start

.macro mbd_p2v offs:req
        movl \offs , %eax
        cmpl $0, %eax
        je _mbd_p2v_\+
        orl $KERNEL_VIRT_BASE, \offs
_mbd_p2v_\+ :
.endm

# Actual bootstrap code (with paging set up)
.section .text
higher_half:
        # First things first, remove the identity mapping
        # note that we can now just use the virtual address
        movl $0, boot_page_directory + 0
        # and flush the tlb
        movl %cr3, %eax
        movl %eax, %cr3

        # Set up the stack
        movl $stack_top, %esp

        # Virtualify multiboot info addrs (if not 0)
        orl $KERNEL_VIRT_BASE, %ebx
        mbd_p2v 0x10(%ebx) # cmdline
        mbd_p2v 0x18(%ebx) # mods_addr
        mbd_p2v 0x24(%ebx) # aout_sym.elf/elf_sec.addr
        mbd_p2v 0x30(%ebx) # mmap_addr
        mbd_p2v 0x38(%ebx) # drives_addr
        mbd_p2v 0x3c(%ebx) # config_table
        mbd_p2v 0x40(%ebx) # boot_loader_name
        mbd_p2v 0x44(%ebx) # apm_table
        mbd_p2v 0x48(%ebx) # vbe_control_info
        mbd_p2v 0x4c(%ebx) # vbe_mode_info
        mbd_p2v 0x58(%ebx) # framebuffer_addr

        # If framebuffer type is indexed, we have an address to modify
        movl 0x6d(%ebx), %eax
        cmpl $0, %eax
        je framebuffer_not_indexed
        mbd_p2v 0x70(%ebx) # framebuffer_palette_addr

framebuffer_not_indexed:
        # Push multiboot info
        push %ebx

        # Run the kernel
        call kernel_main

        # We should never get here, but IF we do let's do something about it
hang:
        cli # Disable interrupts
        hlt # Halt
        jmp hang # If all else fails, busy loop

.size higher_half, . - higher_half
