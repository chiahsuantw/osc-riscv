#include "traps.h"
#include "irq.h"
#include "printk.h"
#include "syscall.h"
#include "vm.h"

void do_traps(struct pt_regs *regs)
{
    if (regs->cause >= 12 && regs->cause <= 15) {
        do_page_fault(regs);
        return;
    }

    if (regs->cause != 8) {
        printk("[PANIC] Unknown exception\n");
        printk("sepc: %p\n", regs->epc);
        printk("sstatus: %p\n", regs->status);
        printk("scause: %p\n", regs->cause);
        printk("stval: %p\n", regs->badaddr);
        while (1)
            ;
    }

    enable_interrupt();

    regs->epc += 4;
    switch (regs->a7) {
    case 0:
        regs->a0 = sys_getpid();
        break;
    case 1:
        regs->a0 = sys_read((char *)regs->a0, regs->a1);
        break;
    case 2:
        regs->a0 = sys_write((const char *)regs->a0, regs->a1);
        break;
    case 3:
        // TODO: sys_exec
        break;
    case 4:
        regs->a0 = sys_fork(regs);
        break;
    case 5:
        sys_exit(0);
        break;
    case 6:
        regs->a0 = sys_kill(regs->a0);
        break;
    case 10:
        regs->a0 = sys_mmap(regs->a0, regs->a1, regs->a2, regs->a3);
        break;
    default:
        printk("[PANIC] Unknown syscall(%d)\n", regs->a7);
    }
}
