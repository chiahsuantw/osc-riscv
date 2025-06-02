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
    case SYS_GETPID:
        regs->a0 = sys_getpid();
        break;
    case SYS_UART_READ:
        regs->a0 = sys_uart_read((char *)regs->a0, regs->a1);
        break;
    case SYS_UART_WRITE:
        regs->a0 = sys_uart_write((const char *)regs->a0, regs->a1);
        break;
    case SYS_EXEC:
        // TODO: sys_exec
        break;
    case SYS_FORK:
        regs->a0 = sys_fork(regs);
        break;
    case SYS_EXIT:
        sys_exit(0);
        break;
    case SYS_KILL:
        struct task_struct *task = find_task_by_pid(regs->a0);
        if (!task) {
            regs->a0 = -1;
            break;
        }
        kthread_stop(task);
        regs->a0 = 0;
        break;
    case SYS_SIGNAL:
        regs->a0 = sys_signal(regs->a0, (void (*)())regs->a1);
        break;
    case SYS_SIGRETURN:
        regs->a0 = sys_sigreturn(regs);
        break;
    case SYS_SIG_KILL:
        regs->a0 = sys_kill(regs->a0, regs->a1);
        break;
    case SYS_MMAP:
        regs->a0 = sys_mmap(regs->a0, regs->a1, regs->a2, regs->a3);
        break;
    case SYS_OPEN:
        regs->a0 = sys_open((const char *)regs->a0, regs->a1);
        break;
    case SYS_CLOSE:
        regs->a0 = sys_close(regs->a0);
        break;
    case SYS_READ:
        regs->a0 = sys_read(regs->a0, (char *)regs->a1, regs->a2);
        break;
    case SYS_WRITE:
        regs->a0 = sys_write(regs->a0, (const char *)regs->a1, regs->a2);
        break;
    case SYS_MKDIR:
        regs->a0 = sys_mkdir((const char *)regs->a0);
        break;
    case SYS_MOUNT:
        regs->a0 = sys_mount((const char *)regs->a0, (const char *)regs->a1,
                             (const char *)regs->a2);
        break;
    case SYS_CHDIR:
        regs->a0 = sys_chdir((const char *)regs->a0);
        break;
    default:
        printk("[PANIC] Unknown syscall(%d)\n", regs->a7);
    }

    do_signal(regs);
}
