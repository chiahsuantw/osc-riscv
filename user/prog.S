.section ".text"
.global _start
_start:
    li a7, 0
    li t0, 0
    li t1, 5
1:
    addi t0, t0, 1
    ecall
    blt t0, t1, 1b
1:
    j 1b
