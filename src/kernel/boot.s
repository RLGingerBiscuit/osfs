# kernel_main is where our C code starts
.extern kernel_main

# Kernel constants
.set KERNEL_VIRT_BASE, 0xC0000000
.set KERNEL_MAX_SIZE, 0x100000 # 1MiB

.set VGA_ADDRESS, 0xB8000

.set PAGE_SIZE, 0x1000 # 4KiB
.set PAGE_PRESENT, 1 << 0
.set PAGE_RW, 1 << 1

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
.skip 4096
.global boot_page_table
boot_page_table:
.skip 4096

.extern multiboot_info_ptr
.type multiboot_info_ptr, @object

.macro mb_p2v offs:req
        movl \offs , %eax
        cmpl $0, %eax
        je _mb_p2v_\+
        orl $KERNEL_VIRT_BASE, \offs
_mb_p2v_\+ :
.endm

# Bootloader/paging setup
.section .boot, "a", @progbits
.global _start
.type _start, @function
_start:
        # NOTE: we CANNOT use ebx since that contains our multiboot info

        # Loop variable (<<2 for pde index, <<12 for address)
        movl $0, %esi
        # Static amount (1MiB for now) to keep it simple
        movl $(KERNEL_MAX_SIZE >> 10), %ecx

        # esi = loop var
        # edi = pde addr
        # edx = phys addr
.map_loop:
        # pde = pt + 4*i
        movl %esi, %edi
        sall $2, %edi
        addl $(boot_page_table - KERNEL_VIRT_BASE), %edi

        # phys = 4096*i | 0x3
        movl %esi, %edx
        sall $12, %edx
        orl $(PAGE_PRESENT | PAGE_RW), %edx

        # *pde = phys
        movl %edx, (%edi)

        # while(++i < KERNEL_MAX_SIZE >> 10)
        addl $1, %esi
        cmpl %esi, %ecx
        jg .map_loop

        # Explicitly map VGA at end of table
        movl $(VGA_ADDRESS | PAGE_PRESENT | PAGE_RW), boot_page_table - KERNEL_VIRT_BASE + (PAGE_SIZE >> 2 - 1)*4

        # Identity map
        movl $(boot_page_table - KERNEL_VIRT_BASE + PAGE_PRESENT + PAGE_RW), boot_page_directory - KERNEL_VIRT_BASE + 0
        # Higher half map
        movl $(boot_page_table - KERNEL_VIRT_BASE + PAGE_PRESENT + PAGE_RW), boot_page_directory - KERNEL_VIRT_BASE + (KERNEL_VIRT_BASE >> 0x14)

        # Tell CPU about our page directory (in physical space)
        movl $(boot_page_directory - KERNEL_VIRT_BASE), %ecx
        movl %ecx, %cr3

        # Enable paging and write-protect
        movl %cr0, %eax
        orl $0x80010000, %eax
        movl %eax, %cr0

        # Virtualify multiboot info addrs (if not 0)
        orl $KERNEL_VIRT_BASE, %ebx
        mb_p2v 0x10(%ebx) # cmdline
        mb_p2v 0x18(%ebx) # mods_addr
        mb_p2v 0x24(%ebx) # aout_sym.elf/elf_sec.addr
        mb_p2v 0x30(%ebx) # mmap_addr
        mb_p2v 0x38(%ebx) # drives_addr
        mb_p2v 0x3c(%ebx) # config_table
        mb_p2v 0x40(%ebx) # boot_loader_name
        mb_p2v 0x44(%ebx) # apm_table
        mb_p2v 0x48(%ebx) # vbe_control_info
        mb_p2v 0x4c(%ebx) # vbe_mode_info
        mb_p2v 0x58(%ebx) # framebuffer_addr

        # If framebuffer type is indexed, we have an address to modify
        movl 0x6d(%ebx), %eax
        cmpl $0, %eax
        je .framebuffer_not_indexed
        mb_p2v 0x70(%ebx) # framebuffer_palette_addr

.framebuffer_not_indexed:
        # Store multiboot info so we don't accidentally clobber it
        movl %ebx, multiboot_info_ptr

        # Jump up to our kernel bootstrap
        lea higher_half, %eax
        jmp *%eax

.size _start, . - _start

# Actual bootstrap code (with paging set up)
.section .text
higher_half:
        # First things first, remove the identity mapping
        # note that we can now just use the virtual address
        movl $0, boot_page_directory
        # and flush the tlb
        movl %cr3, %eax
        movl %eax, %cr3

        # Set up the stack
        movl $stack_top, %esp

        # Run the kernel
        call kernel_main

        # We should never get here, but IF we do let's do something about it
hang:
        cli # Disable interrupts
        hlt # Halt
        jmp hang # If all else fails, busy loop

.size higher_half, . - higher_half
