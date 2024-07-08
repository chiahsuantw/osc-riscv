SECTIONS
{
	. = 0x80000000;
	.text : { *(.text.boot) *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	. = ALIGN(16);
	. = . + 0x2000;
	__stack_top = .;
    _end = .;
}
