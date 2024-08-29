#!/bin/sh

# This script sends the kernel image to VF2 via UART

DEST_PATH=$1
KERNEL_PATH=$2

# Check the root permission
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root"
    exit 1
fi

# Get the size of the kernel image file and send it to Rpi3
# wc -c: count bytes of a file
# sleep: wait n seconds
wc -c < $KERNEL_PATH > $DEST_PATH | sleep 1

# Send the kernel image
# pv: redirect file input to specified tty
#     add --rate-limit <n> option to limit the speed
pv $KERNEL_PATH > $DEST_PATH
