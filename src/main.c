#include "mm.h"
#include "printk.h"
#include "shell.h"

int start_kernel()
{
    mem_init();
    printk("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
