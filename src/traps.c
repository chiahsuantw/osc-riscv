#include "traps.h"
#include "printk.h"

void trap_handler()
{
    printk("Kernel Trap!\n");
    while (1)
        ;
}
