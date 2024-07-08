.section ".text.boot"

.global _start
_start:
    csrr t0, mhartid
    bnez t0, halt
    la sp, _start
    j start_kernel

halt:
    wfi
    j halt
