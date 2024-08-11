#include "printk.h"
#include "shell.h"

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
