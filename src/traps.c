#include "traps.h"
#include "irq.h"
#include "printk.h"
#include "sched.h"
#include "syscall.h"
#include "traps.h"
#include "vm.h"

void do_traps(struct pt_regs *regs)
{
    if (regs->cause == EXC_INST_PAGE_FAULT ||
        regs->cause == EXC_LOAD_PAGE_FAULT ||
        regs->cause == EXC_STORE_PAGE_FAULT) {
        do_page_fault(regs);
        return;
    }

    if (regs->cause != EXC_SYSCALL) {
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
        struct task_struct *task = find_task_by_pid(regs->a0);
        if (!task) {
            regs->a0 = -1;
            break;
        }
        kthread_stop(task);
        regs->a0 = 0;
        break;
    case 7:
        regs->a0 = sys_signal(regs->a0, (void (*)())regs->a1);
        break;
    case 8:
        regs->a0 = sys_sigreturn(regs);
        break;
    case 9:
        regs->a0 = sys_kill(regs->a0, regs->a1);
        break;
    case 10:
        regs->a0 = sys_mmap(regs->a0, regs->a1, regs->a2, regs->a3);
        break;
    default:
        printk("[PANIC] Unknown syscall(%d)\n", regs->a7);
    }

    do_signal(regs);
}
