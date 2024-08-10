RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -I include -ffreestanding -mcmodel=medany -Wall -g
QEMUFLAGS = -M virt -display none -serial stdio \
			-kernel build/kernel.img \
			-initrd res/initramfs.cpio
LD_SCRIPT = src/kernel.ld

SRC_FILES = $(shell find src -name "*.S") \
			$(shell find src -name "*.c")
OBJ_FILES = $(addprefix build/, $(addsuffix .o, \
			$(basename $(notdir $(SRC_FILES)))))

all: clean kernel

clean:
	rm -rf build *.fit

build:
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILES)
	@mkdir -p build
	@mv *.o build

kernel: build
	$(RISCVGUN)-ld -T $(LD_SCRIPT) -o build/kernel.elf $(OBJ_FILES)
	$(RISCVGUN)-objcopy -O binary build/kernel.elf build/kernel.img
	mkimage -f src/kernel.its kernel.fit > /dev/null

qemu: CFLAGS += -D __QEMU__
qemu: LD_SCRIPT = src/qemu.ld
qemu: all
	clear & qemu-system-riscv64 $(QEMUFLAGS)

debug: CFLAGS += -D __QEMU__
debug: LD_SCRIPT = src/qemu.ld
debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -S -s
