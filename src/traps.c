#include "traps.h"
#include "irq.h"
#include "printk.h"

__attribute__((interrupt("supervisor"))) void trap_handler()
{
    printk("Kernel Trap!\n");
    while (1)
        ;
    unsigned long scause;
    asm volatile("csrr %0, scause" : "=r"(scause));
    switch (scause) {
    case 8:
        printk("syscall\n");
        break;
    }
    // irq_entry();
}
