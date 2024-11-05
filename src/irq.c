
#include "irq.h"

void enable_interrupt()
{
    // Set sstatus.SIE to 1
    asm("csrsi sstatus, (1 << 1)");
}

void disable_interrupt()
{
    // Set sstatus.SIE to 0
    asm("csrci sstatus, (1 << 1)");
}

void switch_to_user_mode()
{
    asm("csrw sepc, ra;");
    asm("sret");
}
