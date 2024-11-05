#include "irq.h"
#include "mm.h"
#include "printk.h"
#include "shell.h"
#include "timer.h"

int start_kernel()
{
    mem_init();
    timer_init();
    enable_interrupt();
    set_timeout("World!", 20);
    set_timeout("Hello", 10);
    set_timeout(" ", 15);
    printk("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
