.section ".text.boot"

.global _start
_start:
    /* a0: hartid, a1: dtb */
    la a2, 1f
    li a3, 0xffffffc000000000
    add a2, a2, a3
    csrw stvec, a2

#ifdef __QEMU__
    li a2, 0x80100800
    li a3, 0x000000cf
    sd a3, 0(a2)

    li a2, 0x80100810
    li a3, 0x200000cf
    sd a3, 0(a2)

    li a2, 0x8000000000080100
#else
    li a2, 0x40100800
    li a3, 0x000000cf
    sd a3, 0(a2)

    li a2, 0x40100808
    li a3, 0x100000cf
    sd a3, 0(a2)

    li a2, 0x8000000000040100
#endif
    sfence.vma
    csrw satp, a2

.align 2
1:
    la t0, DTB_BASE
    sd a1, 0(t0)

    la t0, handle_exception
    csrw stvec, t0

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
