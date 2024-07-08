.section ".text.boot"

.global _start
_start:
    csrr t0, mhartid
    bnez t0, halt
    la sp, _start
    la t0, __bss_start
    la t1, __bss_end

bss_reset:
    bge t0, t1, run_kernel
    sd zero, 0(t0)
    addi t0, t0, 8
    j bss_reset

run_kernel:
    call start_kernel

halt:
    wfi
    j halt
