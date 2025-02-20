#include "signal.h"
#include "mm.h"
#include "sched.h"

// FIXME: Map sigreturn to user space
void sigreturn(void)
{
    asm("li a7, 8;"
        "ecall;");
}

long sys_signal(int sig, void (*handler)())
{
    get_current()->sighand[sig] = handler;
    return 0;
}

long sys_sigreturn(struct pt_regs *regs)
{
    // TODO: Check if the frame is at regs->sp
    struct sigframe *frame = (struct sigframe *)regs->sp;
    int signo = 0;
    copy_user(regs, &frame->ucontext, sizeof(struct pt_regs));
    copy_user(&signo, &frame->signo, sizeof(int));
    get_current()->blocked &= ~(1 << signo);
    return regs->a0;
}

long sys_kill(long pid, int sig)
{
    struct task_struct *task = find_task_by_pid(pid);
    if (!task)
        return -1;
    if (task->blocked & (1 << sig))
        return 0;
    task->blocked |= (1 << sig);
    task->pending |= (1 << sig);
    return 0;
}

void do_signal(struct pt_regs *regs)
{
    for (int sig = 0; sig < _NSIG; sig++) {
        if (get_current()->pending & (1 << sig)) {
            get_current()->pending &= ~(1 << sig);

            if (get_current()->sighand[sig] == SIG_DFL) {
                kthread_exit(0);
                get_current()->blocked &= ~(1 << sig);
                return;
            }

            /* Set up the stack frame */
            struct sigframe *frame =
                (struct sigframe *)(regs->sp - sizeof(struct sigframe));
            copy_user(&frame->signo, &sig, sizeof(int));
            copy_user(&frame->ucontext, regs, sizeof(struct pt_regs));
            regs->epc = (unsigned long)get_current()->sighand[sig];
            regs->ra = (unsigned long)sigreturn;
            regs->sp = (unsigned long)frame & ~0xfUL;
            return;
        }
    }
}
