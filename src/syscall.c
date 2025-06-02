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

long sys_uart_read(char *buf, long count)
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

long sys_uart_write(const char *buf, long count)
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
    childregs->a0 = 0;
    child->thread.ra = (unsigned long)ret_from_exception;
    child->thread.sp = (unsigned long)childregs;

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

long sys_open(const char *pathname, int flags)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    char path[PATH_MAX] = { 0 };
    realpath(pathname, path);
    for (int i = 0; i < MAX_FD; i++)
        if (!get_current()->fdt[i])
            if (vfs_open(path, flags, &get_current()->fdt[i]) == 0)
                return i;
    return -1;
}

long sys_close(int fd)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    if (get_current()->fdt[fd]) {
        vfs_close(get_current()->fdt[fd]);
        get_current()->fdt[fd] = 0;
        return 0;
    }
    return -1;
}

long sys_read(int fd, char *buf, long count)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    if (get_current()->fdt[fd])
        return vfs_read(get_current()->fdt[fd], buf, count);
    return -1;
}

long sys_write(int fd, const char *buf, long count)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    if (get_current()->fdt[fd])
        return vfs_write(get_current()->fdt[fd], buf, count);
    return -1;
}

long sys_mkdir(const char *pathname)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    char path[PATH_MAX] = { 0 };
    realpath(pathname, path);
    return vfs_mkdir(path);
}

long sys_mount(const char *source, const char *target, const char *filesystem)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    char path[PATH_MAX] = { 0 };
    realpath(target, path);
    return vfs_mount(path, filesystem);
}

long sys_chdir(const char *path)
{
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    char buf[PATH_MAX] = { 0 };
    realpath(path, buf);
    memset(get_current()->cwd, 0, PATH_MAX);
    strncpy(get_current()->cwd, buf, strlen(buf));
    return 0;
}
