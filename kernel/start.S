.section ".text.boot"

.global _start
_start:
    csrr t0, mhartid
    bnez t0, halt
    j start_kernel

halt:
    wfi
    j halt
