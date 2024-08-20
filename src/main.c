#include "devtree.h"
#include "printk.h"
#include "shell.h"

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");
    fdt_traverse("linux,initrd-start", 0);
    run_shell();
    return 0;
}
