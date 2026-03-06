CC = gcc
ASM = as
LD = ld

X86_BOOTLOADER_DIR = Sailfish-x86-Boot/
BIN_DIR = bin
BIN_KERN_DIR = bin/kernel
BIN_BOOT_DIR = bin/bootloader
OBJ_DIR = obj
IMG_DIR = img
SRC_DIR = src
INC_DIR = ./src/include

C_FLAGS := -I$(INC_DIR) -ffreestanding -Wall -Wextra \
         -mno-red-zone -fno-stack-protector -fno-pic \
         -m64 -march=x86-64 -mabi=sysv -mcmodel=kernel -c -o

C_FLAGS_DEBUG := -I$(INC_DIR) -ffreestanding -Wall -Wextra \
         -mno-red-zone -fno-stack-protector -fno-pic \
         -m64 -march=x86-64 -mabi=sysv -mcmodel=kernel -DDEBUG=1 -g -c -o

SOURCES := $(wildcard $(SRC_DIR)/*.c)

x86: kernel x86-boot
x86-debug: kernel-debug x86-boot

x86-boot: config
	cd ../$(X86_BOOTLOADER_DIR) && $(MAKE) boot-image

kernel: config
	$(foreach src, $(wildcard $(SRC_DIR)/*.c), $(CC) $(src) $(C_FLAGS) $(OBJ_DIR)/$(notdir $(basename $(src))).o;)
	$(LD) $(foreach src, $(SOURCES), $(OBJ_DIR)/$(notdir $(basename $(src))).o) $(LD_FLAGS) -T link.ld -o $(BIN_KERN_DIR)/kernel.elf

kernel-debug: config
	$(foreach src, $(wildcard $(SRC_DIR)/**/*.c), $(CC) $(src) $(C_FLAGS_DEBUG) $(OBJ_DIR)/$(notdir $(basename $(src))).o;)
	$(LD) $(foreach src, $(SOURCES), $(OBJ_DIR)/$(notdir $(basename $(src))).o) $(LD_FLAGS) -T link.ld -o $(BIN_KERN_DIR)/kernel.elf

config:
	mkdir -p bin/
	mkdir -p bin/kernel/
	mkdir -p bin/bootloader/
	mkdir -p obj/
	mkdir -p img/

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(IMG_DIR)