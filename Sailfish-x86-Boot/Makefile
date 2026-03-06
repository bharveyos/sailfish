CC = gcc
ASM = as
LD = ld

BIN_DIR := ../bin
BIN_KERN_DIR := ../bin/kernel
BIN_BOOT_DIR := ../bin/bootloader
OBJ_DIR := ../obj
IMG_DIR := ../img
SRC_DIR := ../src
HOME = /home/bnh
UEFI_DIR = $(HOME)/git/gnu-efi/inc
MOD_DIR := ./modules

UEFI := uefi
MODS := modules

C_FLAGS := -I$(UEFI_DIR) -I$(MOD_DIR) -fpic -ffreestanding -fno-stack-protector \
		 -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c -ggdb -o
LD_FLAGS := -T$(HOME)/git/gnu-efi/gnuefi/elf_x86_64_efi.lds -shared -Bsymbolic -L$(HOME)/git/gnu-efi/x86_64/lib \
		 -L$(HOME)/git/gnu-efi/x86_64/gnuefi -o
EFI_FLAGS := -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* \
		 -j .reloc --target efi-app-x86_64 --subsystem=10

DEBUG_SECTIONS := .debug_info .debug_abbrev .debug_loc .debug_aranges \
	.debug_line .debug_macinfo .debug_str
SECTIONS := .text .sdata .data .dynamic .dynsym .rel .rela .reloc

SOURCES := main.c

boot-image: main-efi debug-efi
	dd if=/dev/zero of=$(IMG_DIR)/boot.img bs=512 count=93750
	parted $(IMG_DIR)/boot.img -s -a minimal mklabel gpt
	parted $(IMG_DIR)/boot.img -s -a minimal mkpart EFI FAT32 2048s 93716s
	parted $(IMG_DIR)/boot.img -s -a minimal toggle 1 boot
	dd if=/dev/zero of=$(IMG_DIR)/tmp.img bs=512 count=91669
	mformat -i $(IMG_DIR)/tmp.img -h 32 -t 32 -n 64 -c 1
	
	mmd -i $(IMG_DIR)/tmp.img ::/EFI
	mmd -i $(IMG_DIR)/tmp.img ::/EFI/BOOT
	mmd -i $(IMG_DIR)/tmp.img ::/EFI/Sailfish
	
	mcopy -i $(IMG_DIR)/tmp.img $(BIN_BOOT_DIR)/BOOTX64.efi ::/EFI/BOOT/
	mcopy -i $(IMG_DIR)/tmp.img DEMO.txt ::/EFI/Sailfish/
	mcopy -i $(IMG_DIR)/tmp.img $(BIN_KERN_DIR)/kernel.elf ::/EFI/Sailfish/
	
	dd if=$(IMG_DIR)/tmp.img of=$(IMG_DIR)/boot.img bs=512 count=91669 seek=2048 conv=notrunc

main-efi:
	$(foreach src, $(wildcard $(UEFI)/*.c), $(CC) $(src) $(C_FLAGS) $(OBJ_DIR)/$(notdir $(basename $(src))).o;)
	$(LD) $(HOME)/git/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o $(foreach src, $(SOURCES), $(OBJ_DIR)/$(basename $(src)).o) $(LD_FLAGS) $(BIN_BOOT_DIR)/main.so -lgnuefi -lefi
	objcopy $(EFI_FLAGS) $(BIN_BOOT_DIR)/main.so $(BIN_BOOT_DIR)/BOOTX64.efi

debug-efi:
	objcopy $(foreach sec, $(SECTIONS) $(DEBUG_SECTIONS), -j $(sec)) --target efi-app-x86_64 $(BIN_BOOT_DIR)/main.so $(BIN_BOOT_DIR)/BOOTX64_DEBUG.efi