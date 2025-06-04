SRC_DIR=src
BUILD_DIR=build

# Sorry, I'm using WSL
QEMU=qemu-system-i386.exe

KERNEL_ELF=$(BUILD_DIR)/kernel.elf
LINKER_FILE=linker.ld

CC=i686-elf-gcc
COMMON_FLAGS=-ffreestanding -g
CFLAGS=$(COMMON_FLAGS) -std=c99 -Wall -Werror -I$(SRC_DIR)
LDFLAGS=$(COMMON_FLAGS) -nostdlib

# This mess takes a path & file exts and recursively matches them (e.g. src/a.c src/utils/b.c)
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
# This one substitues a file ext for .o... that's literally it
objsubst=$(patsubst $(SRC_DIR)/$(subst *,%,$1),$(BUILD_DIR)/$(SRC_DIR)/%.o,$2)

# Calling all c and asm files, we want you
SRCS=$(call rwildcard,$(SRC_DIR),*.c *.s)
# But not really, we want your object files
OBJS=$(call objsubst,*.s,$(call objsubst,*.c,$(SRCS)))

.PHONY=all
all: $(KERNEL_ELF)

.PHONY=run
run: all
	$(QEMU) -kernel $(KERNEL_ELF)

$(KERNEL_ELF): $(BUILD_DIR) $(LINKER_FILE) $(OBJS)
	$(CC) $(LDFLAGS) -T $(LINKER_FILE) $(OBJS) -o $(KERNEL_ELF) -lgcc

# Just build em all (into objects at least)
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# For now we're just 32-bit so we simply just include asm too
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
