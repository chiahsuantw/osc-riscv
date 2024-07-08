RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -I include -Wall -mcmodel=medany -g
QEMUFLAGS = -M virt -bios none -display none -serial stdio \
			-kernel kernel.img

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
	$(RISCVGUN)-objcopy -O binary build/kernel.elf kernel.img

qemu: all
	clear & qemu-system-riscv64 $(QEMUFLAGS)

debug: all
	qemu-system-riscv64 $(QEMUFLAGS) -S -s
