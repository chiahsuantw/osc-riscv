#include "traps.h"
#include "irq.h"
#include "printk.h"

void trap_handler()
{
    irq_entry();
    // printk("Kernel Trap!\n");
    // while (1)
    //     ;
}
