#include "syscall.h"
#include "irq.h"
#include "mm.h"
#include "sched.h"
#include "string.h"
#include "traps.h"
#include "uart.h"
#include "vm.h"

extern void ret_from_exception();

long sys_getpid()
{
    return get_current()->pid;
}

long sys_read(char *buf, long count)
{
    // Set sstatus.SUM to 1
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    int i = 0;
    while (i < count)
        buf[i++] = uart_getc();
    // Set sstatus.SUM to 0
    asm("li t0, (1 << 18);"
        "csrc sstatus, t0;");
    return i;
}

long sys_write(const char *buf, long count)
{
    // Set sstatus.SUM to 1
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    int i = 0;
    while (i < count)
        uart_putc(buf[i++]);
    // Set sstatus.SUM to 0
    asm("li t0, (1 << 18);"
        "csrc sstatus, t0;");
    return i;
}

long sys_exec(const char *filename, const char *const *argv)
{
    return 0;
}

long sys_fork(struct pt_regs *regs)
{
    disable_interrupt();
    struct task_struct *parent = get_current();
    struct task_struct *child = kthread_create(0);

    // Copy kernel stack
    memcpy((void *)child->stack, (void *)parent->stack, STACK_SIZE);
    // Copy VMA list, increase the refcount, and mark the pages as read-only
    dup_mmap(&child->mm, &parent->mm);
    // Copy signal handlers
    memcpy(child->sighand, parent->sighand, sizeof(parent->sighand));

    unsigned long sp_offset = (unsigned long)regs - parent->stack;
    struct pt_regs *childregs = (struct pt_regs *)(child->stack + sp_offset);
    child->thread.ra = (unsigned long)ret_from_exception;
    child->thread.sp = (unsigned long)childregs;
    childregs->sp = regs->sp;
    childregs->a0 = 0;

    enable_interrupt();
    return child->pid;
}

long sys_exit(int error_code)
{
    kthread_exit();
    return 0;
}

// long sys_kill(long pid)
// {
//     struct task_struct *task = find_task_by_pid(pid);
//     kthread_stop(task);
//     return 0;
// }

long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
              unsigned long flags)
{
    return 0;
}
