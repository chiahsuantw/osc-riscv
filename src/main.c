#include "devtree.h"
#include "printk.h"
#include "shell.h"

void switch_to_user_mode()
{
    asm("csrw sepc, ra;");
    asm("sret");
}

void trap_handler()
{
    printk("Kernel Trap!\n");
    while (1)
        ;
}

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");
    switch_to_user_mode();
    asm("ecall");
    run_shell();
    return 0;
}
