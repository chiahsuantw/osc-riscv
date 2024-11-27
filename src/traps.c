#include "traps.h"
#include "irq.h"
#include "printk.h"

void trap_handler(struct pt_regs *regs)
{
    printk("Kernel Trap!\n");
    // while (1)
    //     ;
    // unsigned long scause;
    // asm volatile("csrr %0, scause" : "=r"(scause));
    // switch (scause) {
    // case 8:
    //     printk("syscall\n");
    //     break;
    // }
    do_irq(0);
}
