SRC_DIR=src
BUILD_DIR=build
ISO_DIR=$(BUILD_DIR)/isodir

KERNEL_DIR=$(SRC_DIR)/kernel
LIBK_DIR=$(SRC_DIR)/libk
LIBC_DIR=$(SRC_DIR)/libc

IS_WSL=$(shell command -v wslpath 2>/dev/null)

QEMU=$(if $(IS_WSL),qemu-system-i386.exe,qemu-system-x86)

LINKER_FILE=linker.ld
GRUB_CFG=grub.cfg

KERNEL_ELF=$(BUILD_DIR)/kernel.elf
KERNEL_ISO=$(BUILD_DIR)/kernel.iso

CC=i686-elf-gcc
COMMON_FLAGS=-ffreestanding -g
CFLAGS=$(COMMON_FLAGS) -std=c99 -fno-stack-protector -fno-builtin -Wall -Werror -I$(SRC_DIR)/kernel/include -I$(SRC_DIR)/libc/include
LDFLAGS=$(COMMON_FLAGS) -nostdlib

LIBK_CFLAGS=$(CFLAGS) -DOSFS_LIBK
LIBC_CFLAGS=$(CFLAGS) -DOSFS_LIBC

# This mess takes a path & file exts and recursively matches them (e.g. src/a.c src/utils/b.c)
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
# This one prepends 'build/' and appends '.o' ... that's literally it
objsubst=$(patsubst $(SRC_DIR)/$(subst *,%,$1),$(BUILD_DIR)/$(SRC_DIR)/%$(subst *,,$1).o,$2)

KERNEL_SRCS=$(call rwildcard,$(KERNEL_DIR),*.c *.S)
LIBC_SRCS=$(call rwildcard,$(LIBC_DIR),*.c)
LIBK_SRCS=$(call rwildcard,$(LIBK_DIR),*.c)

KERNEL_OBJS=$(call objsubst,*.S,$(call objsubst,*.c,$(KERNEL_SRCS)))
LIBC_OBJS=$(call objsubst,*.c,$(LIBC_SRCS))
LIBK_OBJS=$(call objsubst,*.c,$(LIBK_SRCS)) $(patsubst $(BUILD_DIR)/$(LIBC_DIR)/%,$(BUILD_DIR)/$(LIBK_DIR)/%,$(LIBC_OBJS))

.PHONY=all
all: $(KERNEL_ISO)

.PHONY=run
run: all
ifeq ($(IS_WSL),)
	$(QEMU) -cdrom '$(KERNEL_ISO)' -monitor stdio
else
	$(QEMU) -cdrom '$(shell wslpath -a -w $(KERNEL_ISO))' -monitor stdio
endif

# Make an ISO with the kernel binary
$(KERNEL_ISO): $(KERNEL_ELF) $(GRUB_CFG)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot
	cp $(GRUB_CFG) $(ISO_DIR)/boot/grub
	grub-mkrescue -o $(KERNEL_ISO) $(ISO_DIR)
	@if grub-file --is-x86-multiboot $(KERNEL_ELF); then echo 'Kernel is multiboot'; else echo 'Kernel is not multiboot'; fi

# Link kernel binary
$(KERNEL_ELF): $(BUILD_DIR) $(LINKER_FILE) $(KERNEL_OBJS) $(LIBK_OBJS)
	$(CC) $(LDFLAGS) -T $(LINKER_FILE) $(KERNEL_OBJS) $(LIBK_OBJS) -o $(KERNEL_ELF) -lgcc

# Compile libk (+ libc in 'kernel mode')
$(BUILD_DIR)/$(LIBK_DIR)/%.c.o: $(LIBC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(LIBK_CFLAGS) -c -o $@ $<

# Compile libc (in 'user mode')
$(BUILD_DIR)/$(LIBC_DIR)/%.c.o: $(LIBC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(LIBC_CFLAGS) -c -o $@ $<

# Compile kernel-specific code
$(BUILD_DIR)/$(KERNEL_DIR)/%.c.o: $(KERNEL_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile interrupts in their super special way
$(BUILD_DIR)/$(KERNEL_DIR)/interrupt_handlers.c.o: $(KERNEL_DIR)/interrupt_handlers.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -mgeneral-regs-only -c -o $@ $<

# For now we're just x86 so we just include asm too
$(BUILD_DIR)/$(KERNEL_DIR)/%.S.o: $(KERNEL_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY=clean
clean:
	rm -rf $(BUILD_DIR)
