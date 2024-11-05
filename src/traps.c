#include "traps.h"
#include "printk.h"
#include "timer.h"

void trap_handler()
{
    timer_irq_handler();
    // printk("Kernel Trap!\n");
    // while (1)
    //     ;
}
