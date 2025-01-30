#include "syscall.h"
#include "irq.h"
#include "sched.h"
#include "string.h"
#include "traps.h"
#include "uart.h"

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

int sys_exec(const char *pathname, const char *const *argv)
{
    return 0;
}

long sys_fork(struct pt_regs *regs)
{
    disable_interrupt();
    struct task_struct *parent = get_current();
    struct task_struct *child = kthread_create(0);

    // Copy the kernel stack and the user stack
    memcpy((void *)child->kernel_stack, (void *)parent->kernel_stack,
           STACK_SIZE);
    memcpy((void *)child->user_stack, (void *)parent->user_stack, STACK_SIZE);

    unsigned long sp_off = (unsigned long)regs - parent->kernel_stack;
    struct pt_regs *childregs =
        (struct pt_regs *)(child->kernel_stack + sp_off);
    child->context.ra = (unsigned long)ret_from_exception;
    child->context.sp = (unsigned long)childregs;

    unsigned long user_sp_off = regs->sp - parent->user_stack;
    childregs->sp = child->user_stack + user_sp_off;
    childregs->a0 = 0;

    // RISC-V uses s0 (fp) to calcuate the offset of variables in the stack
    // childregs->s0 = regs->s0 + (child->kernel_stack - parent->kernel_stack);
    // Add an offset to the previous frame pointer
    // *(unsigned long *)(childregs->s0 - 8) +=
    //     (child->kernel_stack - parent->kernel_stack);

    enable_interrupt();
    return child->pid;
}

void sys_exit(int status)
{
    kthread_exit();
}

int sys_kill(long pid)
{
    struct task_struct *task = find_task(pid);
    kthread_stop(task);
    return 0;
}

long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
              unsigned long flags, long offset)
{
    return 0;
}
