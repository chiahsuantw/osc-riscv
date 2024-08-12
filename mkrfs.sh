#!/bin/sh

# Check if the argument is provided
if [ "$#" -eq 0 ]; then
    echo "usage: sh mkrfs.sh [c | d]"
    exit 1
fi

for arg in "$@"; do
    # Compress the rootfs
    if [ "$arg" = "c" ]; then
        cd rootfs
        find . | cpio -o -H newc > ../res/initramfs.cpio
        exit 0
    fi

    # Decompress the rootfs
    if [ "$arg" = "d" ]; then
        mkdir -p rootfs
        cpio -id < res/initramfs.cpio -D rootfs
        exit 0
    fi
done
