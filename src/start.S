.section ".text.boot"

.global _start
_start:
    la t0, DTB_BASE
    sd a1, 0(t0)
    la sp, __stack_top
    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bge t0, t1, run_kernel
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

run_kernel:
    call start_kernel

halt:
    wfi
    j halt
