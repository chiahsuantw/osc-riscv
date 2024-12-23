#include "syscall.h"
#include "irq.h"
#include "sched.h"
#include "string.h"
#include "uart.h"

long sys_getpid()
{
    return get_current()->pid;
}

long sys_read(char *buf, long count)
{
    int i = 0;
    while (i < count)
        buf[i++] = uart_getc();
    return i;
}

long sys_write(const char *buf, long count)
{
    int i = 0;
    while (i < count)
        uart_putc(buf[i++]);
    return i;
}

int sys_exec(const char *pathname, const char *const *argv)
{
    return 0;
}

long sys_fork()
{
    disable_interrupt();
    struct task_struct *parent = get_current();
    struct task_struct *child = kthread_create(0);

    // TODO: Copy the kernel stack and the user stack

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