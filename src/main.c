#include "cmd.h"
#include "irq.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "shell.h"
#include "timer.h"

void foo()
{
    for (int i = 0; i < 5; i++) {
        printk("Thread id: %d %d\n", get_current()->pid, i);
        for (int i = 0; i < 100000000; i++)
            ;
        schedule();
    }
    kthread_exit();
}

int start_kernel()
{
    mem_init();
    // timer_init();
    // irq_init();
    // enable_interrupt();
    kthread_init();

    for (int i = 0; i < 3; i++)
        kthread_create(foo);
    idle();

    cmd_init();
    printk("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
