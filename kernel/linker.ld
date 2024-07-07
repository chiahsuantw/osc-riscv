SECTIONS
{
	. = 0x80000;
    __kernel_start = .;
	.text : { *(.text.boot) *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
    __kernel_end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;
