RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -Iinclude -Wall -g
ASMFLAGS =
QEMUFLAGS = -M virt -nographic

BUILD_DIR = build

all: clean kernel8.img

clean:
	rm -rf $(BUILD_DIR) *.img

build:
	mkdir -p $(BUILD_DIR)

qemu: kernel8.img
	qemu-system-riscv64 $(QEMUFLAGS) -kernel kernel8.img
