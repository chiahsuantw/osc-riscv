#include "cmd.h"
#include "irq.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "shell.h"
#include "syscall.h"
#include "timer.h"
#include "vfs.h"
#include "video.h"

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");
#if __QEMU__
    video_init();
#endif
    mem_init();
    kthread_init();
    timer_init();
    irq_init();
    enable_interrupt();
    vfs_init();
    cmd_init();
    run_shell();
    return 0;
}
