OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
	. = 0x86200000;
	.text : { *(.text.boot) *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	. = ALIGN(16);
	. = . + 0x1000;
	__stack_top = .;
	_end = .;
}
