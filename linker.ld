/* Start @ _start label */
ENTRY(_start)

KERNEL_PHYS_BASE = 1M;
KERNEL_VIRT_BASE = 0xC0000000;
KERNEL_MAX_SIZE = 1M;
PAGE_SIZE = 4K;

SECTIONS
{
  /* Put sections starting at 1M */
  . = KERNEL_PHYS_BASE;

  /* Mark the (physical) start of the kernel */
  _kernel_start = .;

  /* The multiboot header and paging init */
  .boot ALIGN(PAGE_SIZE) : AT(ADDR(.boot))
  {
    KEEP(*(.multiboot))
    *(.boot)
  }

  /* Jump up to our higher half */
  . += KERNEL_VIRT_BASE;

  /* A marker so we know what should be RO */
  _kernel_readonly_start = . - KERNEL_VIRT_BASE;

  /* The code itself */
  .text ALIGN(PAGE_SIZE) : AT(ADDR(.text) - KERNEL_VIRT_BASE)
  {
    *(.text)
  }

  /* RO data */
  .rodata ALIGN(PAGE_SIZE) : AT(ADDR(.rodata) - KERNEL_VIRT_BASE)
  {
    *(.rodata)
  }

  /* A marker so we know what should be RO */
  _kernel_readonly_end = . - KERNEL_VIRT_BASE;

  /* A marker so we know what should be RW */
  _kernel_readwrite_start = . - KERNEL_VIRT_BASE;

  /* RW data */
	.data ALIGN(PAGE_SIZE) : AT(ADDR(.data) - KERNEL_VIRT_BASE)
	{
		*(.data)
	}

	/* uninitialised RW data, stack, and page tables */
	.bss ALIGN(PAGE_SIZE) : AT(ADDR(.bss) - KERNEL_VIRT_BASE)
	{
		*(COMMON)
		*(.bss)
		*(.page_tables)
	}

	_kernel_readwrite_end = . - KERNEL_VIRT_BASE;

	/* Mark the (physical) end of the kernel */
	_kernel_end = . - KERNEL_VIRT_BASE;

	/* Ensure kernel will be mapped correctly (and leave space for VGA) */
	. = ASSERT(_kernel_end - _kernel_start < KERNEL_MAX_SIZE - 4096, "Kernel too large");
}
