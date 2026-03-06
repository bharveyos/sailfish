#!/bin/sh
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
  -s \
  -S \
  -net user,hostfwd=tcp::2222-:22 \
  -drive format=raw,unit=0,file=/home/bnh/git/Sailfish-Kernel/img/boot.img \
  -drive file=second-drive.qcow2,format=qcow2,if=virtio \
  -drive file=third-drive.qcow2,format=qcow2,if=virtio \
  -netdev user,id=net0 \
  -device e1000,netdev=net0 \
  -m 4096M \
  -vnc :1