#include "asm.h"

.global handle_exception
.align 4
handle_exception:
    // TODO: Swich user sp to kernel sp
    // Save context
    addi sp, sp, -PT_SIZE_ON_STACK
    sd x1,  PT_RA(sp)
    sd x3,  PT_GP(sp)
    sd x5,  PT_T0(sp)

    sd x6,  PT_T1(sp)
    sd x7,  PT_T2(sp)
    sd x8,  PT_S0(sp)
    sd x9,  PT_S1(sp)
    sd x10, PT_A0(sp)
    sd x11, PT_A1(sp)
    sd x12, PT_A2(sp)
    sd x13, PT_A3(sp)
    sd x14, PT_A4(sp)
    sd x15, PT_A5(sp)
    sd x16, PT_A6(sp)
    sd x17, PT_A7(sp)
    sd x18, PT_S2(sp)
    sd x19, PT_S3(sp)
    sd x20, PT_S4(sp)
    sd x21, PT_S5(sp)
    sd x22, PT_S6(sp)
    sd x23, PT_S7(sp)
    sd x24, PT_S8(sp)
    sd x25, PT_S9(sp)
    sd x26, PT_S10(sp)
    sd x27, PT_S11(sp)
    sd x28, PT_T3(sp)
    sd x29, PT_T4(sp)
    sd x30, PT_T5(sp)
    sd x31, PT_T6(sp)

    // TODO: sp
    csrrc s1, sstatus, t0
    csrr s2, sepc
    csrr s3, stval
    csrr s4, scause
    csrr s5, sscratch
    # sd s0, PT_SP(sp)
    sd s1, PT_STATUS(sp)
    sd s2, PT_EPC(sp)
    sd s3, PT_BADADDR(sp)
    sd s4, PT_CAUSE(sp)
    sd s5, PT_TP(sp)

    move a0, sp /* pt_regs */

    /*
     * MSB of scause differentiates between
     * interrupts and exceptions
     */
    bge s4, zero, 1f

    /* Handle interrupts */
    call do_irq
    j ret_from_exception
1:
    /* Handle exceptions */
    call do_traps
    j ret_from_exception

.global ret_from_exception
ret_from_exception:
    // Restore context

    ld s1, PT_STATUS(sp)
    ld s2, PT_EPC(sp)
    csrw sstatus, s1
    csrw sepc, s2

    ld x1,  PT_RA(sp)
    ld x3,  PT_GP(sp)
    # ld x4,  PT_TP(sp)
    ld x5,  PT_T0(sp)

    ld x6,  PT_T1(sp)
    ld x7,  PT_T2(sp)
    ld x8,  PT_S0(sp)
    ld x9,  PT_S1(sp)
    ld x10, PT_A0(sp)
    ld x11, PT_A1(sp)
    ld x12, PT_A2(sp)
    ld x13, PT_A3(sp)
    ld x14, PT_A4(sp)
    ld x15, PT_A5(sp)
    ld x16, PT_A6(sp)
    ld x17, PT_A7(sp)
    ld x18, PT_S2(sp)
    ld x19, PT_S3(sp)
    ld x20, PT_S4(sp)
    ld x21, PT_S5(sp)
    ld x22, PT_S6(sp)
    ld x23, PT_S7(sp)
    ld x24, PT_S8(sp)
    ld x25, PT_S9(sp)
    ld x26, PT_S10(sp)
    ld x27, PT_S11(sp)
    ld x28, PT_T3(sp)
    ld x29, PT_T4(sp)
    ld x30, PT_T5(sp)
    ld x31, PT_T6(sp)
    addi sp, sp, PT_SIZE_ON_STACK

    sret

.global switch_to
switch_to:
    /* Save context into prev->context */
    sd ra, CTX_RA(a0)
    sd sp, CTX_SP(a0)
    sd s0, CTX_S0(a0)
    sd s1, CTX_S1(a0)
    sd s2, CTX_S2(a0)
    sd s3, CTX_S3(a0)
    sd s4, CTX_S4(a0)
    sd s5, CTX_S5(a0)
    sd s6, CTX_S6(a0)
    sd s7, CTX_S7(a0)
    sd s8, CTX_S8(a0)
    sd s9, CTX_S9(a0)
    sd s10, CTX_S10(a0)
    sd s11, CTX_S11(a0)

    /* Restore context from next->context */
    ld ra, CTX_RA(a1)
    ld sp, CTX_SP(a1)
    ld s0, CTX_S0(a1)
    ld s1, CTX_S1(a1)
    ld s2, CTX_S2(a1)
    ld s3, CTX_S3(a1)
    ld s4, CTX_S4(a1)
    ld s5, CTX_S5(a1)
    ld s6, CTX_S6(a1)
    ld s7, CTX_S7(a1)
    ld s8, CTX_S8(a1)
    ld s9, CTX_S9(a1)
    ld s10, CTX_S10(a1)
    ld s11, CTX_S11(a1)

    move tp, a1
    ret