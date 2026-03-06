# Connect to QEMU
target remote localhost:1234

# Load kernel symbols
file bin/kernel/kernel.elf

# Set architecture
set architecture i386:x86-64

set disassembly-flavor intel
set pagination off
break kernel_main
layout next
layout next
set trace-commands on
set logging on
continue