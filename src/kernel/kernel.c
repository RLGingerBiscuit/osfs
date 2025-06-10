#include <stddef.h>
#include <stdint.h>

#include <kernel/kstdio.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/vga.h>

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void kernel_main(multiboot_info_t *mbd, uint32_t magic) {
  vga_init();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic!");

  if (mbd->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME && mbd->boot_loader_name)
    vga_printf("Bootloader: %s\n\n", (char *)mbd->boot_loader_name);
  else
    vga_printstr("Unknown bootloader!\n\n");

  if ((mbd->flags & MULTIBOOT_INFO_MEM_MAP) == 0)
    panic("Invalid multiboot memory map!");

  for (uintptr_t i = 0; i < mbd->mmap_length;
       i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mm = (multiboot_memory_map_t *)(mbd->mmap_addr + i);

    vga_printf("Start Addr: 0x%llx | Length: %llx | Size: %x | Type: %x\n",
               mm->addr, mm->len, mm->size, mm->type);

    if (mm->type == MULTIBOOT_MEMORY_AVAILABLE) {
      vga_printf("\tAVAILABLE\n");
    }
  }
}
