RISCVGUN ?= riscv64-unknown-elf
SRC_FILE ?=

CFLAGS = -ffreestanding -mcmodel=medany -Wall -g

all:
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILE)
	$(RISCVGUN)-ld -T user.ld -o $(SRC_FILE:.S=.elf) $(SRC_FILE:.S=.o)
	$(RISCVGUN)-objcopy -O binary $(SRC_FILE:.S=.elf) $(SRC_FILE:.S=.img)

clean:
	rm *.o *.elf *.img
