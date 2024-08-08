RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -I include -Wall -mcmodel=medany -g
QEMUFLAGS = -M virt -display none -serial stdio \
			-kernel build/kernel.img \
			# -dtb res/jh7110-starfive-visionfive-2-v1.3b.dtb

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
	$(RISCVGUN)-ld -T src/kernel.ld -o build/kernel.elf $(OBJ_FILES)
	$(RISCVGUN)-objcopy -O binary build/kernel.elf build/kernel.img
	mkimage -f src/kernel.its kernel.fit > /dev/null

qemu: CFLAGS += -D __QEMU__
qemu: all
	clear & qemu-system-riscv64 $(QEMUFLAGS)

debug: CFLAGS += -D __QEMU__
debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -S -s
