#!/bin/sh
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
  -net user,hostfwd=tcp::2222-:22 \
  -drive format=raw,unit=0,file=/home/bnh/git/sailfish/kernel/img/boot.img \
  -drive file=second-drive.qcow2,format=qcow2,if=virtio \
  -drive file=third-drive.qcow2,format=qcow2,if=virtio \
  -netdev user,id=net0 \
  -device e1000,netdev=net0 \
  -audio none \
  -m 4096M \
  -chardev stdio,id=char0,logfile=serial.log,signal=on \
  -serial chardev:char0 \
  -vnc :0