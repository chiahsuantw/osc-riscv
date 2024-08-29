.section ".text.boot"

.global _start
_start:
    ld sp, __stack_ptr
    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bge t0, t1, relocate
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

relocate:
    la t0, _start
    ld t1, __reloc_start
    ld t2, __reloc_end
1:
    bge t1, t2, run_main
    ld t4, 0(t0)
    sd t4, 0(t1)
    addi t0, t0, 8
    addi t1, t1, 8
    j 1b

run_main:
    call main + 0x6000000

halt:
    wfi
    j halt

.section ".data"
__stack_ptr: .dword __stack_top
__reloc_start: .dword _start
__reloc_end: .dword _end
