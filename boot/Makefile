RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -ffreestanding -mcmodel=medany -Wall -g
QEMUFLAGS = -M virt -smp 5 -m 4G -display none -serial pty \
			-kernel build/bootloader.img \
			-initrd ../res/initramfs.cpio
LD_SCRIPT = boot.ld

all: clean
	mkdir -p build
	$(RISCVGUN)-gcc $(CFLAGS) -c start.S -o build/start.o
	$(RISCVGUN)-gcc $(CFLAGS) -c main.c -o build/main.o
	$(RISCVGUN)-ld -T $(LD_SCRIPT) -o build/bootloader.elf build/start.o build/main.o
	$(RISCVGUN)-objcopy -O binary build/bootloader.elf build/bootloader.img
	mkimage -f boot.its build/kernel.fit

qemu: all
	qemu-system-riscv64 $(QEMUFLAGS)

debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -s -S

clean:
	rm -rf build
