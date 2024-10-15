
#include "irq.h"
#include "printk.h"

void switch_to_user_mode()
{
    asm("csrw sepc, ra;");
    asm("sret");
}

void trap_handler()
{
    printk("Kernel Trap!\n");
    while (1)
        ;
}

void enable_interrupt()
{
    // Set sstatus.SIE to 1
    asm("csrsi sstatus, (1 << 1)");
}

void enable_timer_interrupt()
{
    // Set sie.TSIE to 1
    asm("li t0, (1 << 5);"
        "csrs sie, t0;");
}
