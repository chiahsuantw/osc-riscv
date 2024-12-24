#include "cmd.h"
#include "irq.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "shell.h"
#include "syscall.h"
#include "timer.h"

extern void fork_test();

int start_kernel()
{
    mem_init();
    timer_init();
    irq_init();
    enable_interrupt();
    kthread_init();
    cmd_init();

    kthread_create(fork_test);
    idle();

    printk("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
