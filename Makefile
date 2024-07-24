RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -I include -Wall -mcmodel=medany -g
QEMUFLAGS = -M virt -display none -serial stdio \
			-kernel build/kernel.elf

SRC_FILES = $(shell find src -name "*.S") \
			$(shell find src -name "*.c")
OBJ_FILES = $(addprefix build/, $(addsuffix .o, \
			$(basename $(notdir $(SRC_FILES)))))

all: clean kernel.img

clean:
	@rm -rf build *.img

build:
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILES)
	@mkdir -p build
	@mv *.o build

kernel.img: build
	$(RISCVGUN)-ld -T src/kernel.ld -o build/kernel.elf $(OBJ_FILES)
	$(RISCVGUN)-objcopy -O binary build/kernel.elf build/kernel.bin
	mkimage -A riscv -O linux -T kernel -C none -a 0x40200000 -e 0x40200000 -d build/kernel.bin kernel.img

qemu: all
	clear & qemu-system-riscv64 $(QEMUFLAGS)

debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -S -s
