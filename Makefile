SRC_DIR=src
BUILD_DIR=build
ISO_DIR=$(BUILD_DIR)/isodir

# Sorry, I'm using WSL
QEMU=qemu-system-i386.exe

LINKER_FILE=linker.ld
GRUB_CFG=grub.cfg

KERNEL_ELF=$(BUILD_DIR)/kernel.elf
KERNEL_ISO=$(BUILD_DIR)/kernel.iso

CC=i686-elf-gcc
COMMON_FLAGS=-ffreestanding -g
CFLAGS=$(COMMON_FLAGS) -std=c99 -fno-stack-protector -fno-builtin -Wall -Werror -I$(SRC_DIR)/kernel/include -I$(SRC_DIR)/libc/include -DOSFS_LIBK
LDFLAGS=$(COMMON_FLAGS) -nostdlib

# This mess takes a path & file exts and recursively matches them (e.g. src/a.c src/utils/b.c)
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
# This one substitues a file ext for .o... that's literally it
objsubst=$(patsubst $(SRC_DIR)/$(subst *,%,$1),$(BUILD_DIR)/$(SRC_DIR)/%$(subst *,,$1).o,$2)

# Calling all c and asm files, we want you
SRCS=$(call rwildcard,$(SRC_DIR),*.c *.s)
# But not really, we want your object files
OBJS=$(call objsubst,*.s,$(call objsubst,*.c,$(SRCS)))

.PHONY=all
all: $(KERNEL_ISO)

.PHONY=run
run: all
# Sorry, I'm using WSL
# $(QEMU) -cdrom build/kernel.iso
	$(QEMU) -cdrom '$(shell wslpath -a -w $(KERNEL_ISO))' -monitor stdio

$(KERNEL_ISO): $(KERNEL_ELF) $(GRUB_CFG)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot
	cp $(GRUB_CFG) $(ISO_DIR)/boot/grub
	grub-mkrescue -o $(KERNEL_ISO) $(ISO_DIR)

$(KERNEL_ELF): $(BUILD_DIR) $(LINKER_FILE) $(OBJS)
	$(CC) $(LDFLAGS) -T $(LINKER_FILE) $(OBJS) -o $(KERNEL_ELF) -lgcc

# Just build em all (into objects at least)
$(BUILD_DIR)/$(SRC_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# For now we're just 32-bit so we simply just include asm too
$(BUILD_DIR)/$(SRC_DIR)/%.s.o: $(SRC_DIR)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY=clean
clean:
	rm -rf $(BUILD_DIR)
