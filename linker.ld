/* Start @ _start label */
ENTRY(_start)

SECTIONS
{
  /* Put sections starting at 2M */
  . = 2M;

  /* The code itself */
  .text BLOCK(4K) : ALIGN(4K)
  {
    /* The multiboot header */
    *(.multiboot)
    /* The code itself */
    *(.text)
  }

  /* RO data */
  .rodata BLOCK(4K) : ALIGN(4K)
  {
    *(.rodata)
  }

  /* RW data */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* more RW data & stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
