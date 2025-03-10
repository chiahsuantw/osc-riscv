#include "cmd.h"
#include "irq.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "shell.h"
#include "syscall.h"
#include "timer.h"

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");
    mem_init();
    kthread_init();
    timer_init();
    irq_init();
    enable_interrupt();
    cmd_init();
    run_shell();
    return 0;
}
