/* Start @ _start label */
ENTRY(_start)

KERNEL_PHYS_BASE = 1M;
/* KERNEL_VIRT_BASE = 0xC0000000; */
KERNEL_VIRT_BASE = 0;

SECTIONS
{
  /* Put sections starting at 1M */
  . = KERNEL_PHYS_BASE;

  _kernel_start = .;
  _kernel_phys_start = .;

  /* The multiboot header */
  .multiboot ALIGN(4K) : AT(ADDR(.multiboot))
  {
    KEEP(*(.multiboot.data))
    KEEP(*(.boot))
  }

  _kernel_phys_end = .;

  /* Jump up to our higher half */
  . += KERNEL_VIRT_BASE;

  _kernel_virt_start = .;

  /* The code itself */
  .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRT_BASE)
  {
    *(.text)
  }

  /* RO data */
  .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRT_BASE)
  {
    *(.rodata)
  }

  /* RW data */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRT_BASE)
	{
		*(.data)
	}

	/* uninitialised RW data, stack, and page tables */
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRT_BASE)
	{
		*(COMMON)
		*(.bss)
		*(.page_tables)
	}

	_kernel_virt_end = .;
	_kernel_end = .;
}
