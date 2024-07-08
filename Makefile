RISCVGUN ?= riscv64-unknown-elf

CFLAGS = -Iinclude -Wall -g
QEMUFLAGS = -M virt -bios none -display none -serial null -serial stdio

SRC_FILES = $(shell find src -name "*.S") \
			$(shell find src -name "*.c")
OBJ_FILES = $(addprefix build/, $(addsuffix .o, \
			$(basename $(notdir $(SRC_FILES)))))

all: clean kernel.img

clean:
	rm -rf build *.img

build:
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILES)
	@mkdir -p build
	@mv *.o build

kernel.img: build
	$(RISCVGUN)-ld -T src/kernel.ld -o build/kernel.elf $(OBJ_FILES)
	$(RISCVGUN)-objcopy -O binary build/kernel.elf kernel.img

qemu: kernel.img
	qemu-system-riscv64 $(QEMUFLAGS) -kernel kernel.img -S -s
