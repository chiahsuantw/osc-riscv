RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -I include -ffreestanding -mcmodel=medany -fomit-frame-pointer -Wall -g
QEMUFLAGS = -M virt -smp 5 -m 4G -serial stdio \
			-kernel build/kernel.bin \
			-initrd res/initramfs.cpio \
			-device ramfb
LD_SCRIPT = src/kernel.ld

SRC_FILES = $(shell find src -name "*.S") \
			$(shell find src -name "*.c")
OBJ_FILES = $(addprefix build/, $(addsuffix .o, \
			$(basename $(notdir $(SRC_FILES)))))

all: clean kernel

clean:
	rm -rf build *.fit *.o

build:
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILES)
	@mkdir -p build
	@mv *.o build

kernel: build
	$(RISCVGUN)-gcc -T $(LD_SCRIPT) -nostdlib -o build/kernel.elf $(OBJ_FILES) -lgcc
	$(RISCVGUN)-objcopy -O binary build/kernel.elf build/kernel.bin
	mkimage -f src/kernel.its kernel.fit > /dev/null

qemu: CFLAGS += -D __QEMU__
qemu: LD_SCRIPT = src/qemu.ld
qemu: all
	clear & qemu-system-riscv64 $(QEMUFLAGS)

debug: CFLAGS += -D __QEMU__
debug: LD_SCRIPT = src/qemu.ld
debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -S -s
