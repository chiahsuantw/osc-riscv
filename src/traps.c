#include "traps.h"
#include "irq.h"
#include "printk.h"

void do_traps(struct pt_regs *regs)
{
    printk("do_traps\n");
    unsigned long scause;
    asm volatile("csrr %0, scause" : "=r"(scause));
    switch (scause) {
    case 8:
        printk("syscall\n");
        break;
    }
    regs->epc += 4;
}
