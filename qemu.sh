#!/bin/sh

QEMU_EDI=/home/jonass/dev/bachelor/qemu-tests/qemu_edi/build/qemu-system-arm

if [ ! -f $QEMU_EDI ]; then
    echo "Could not find qemu_edi binary"
    exit 1
fi

$QEMU_EDI -machine virt_cortex_m \
    -semihosting -semihosting-config enable=on,target=native \
    -monitor null \
    -kernel build/app/qemu_runner/app_qemu.elf -device kp-edi-group
