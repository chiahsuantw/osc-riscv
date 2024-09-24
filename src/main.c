#include "devtree.h"
#include "printk.h"
#include "sbi.h"
#include "shell.h"
#include "uart.h"

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

void enable_interrupt()
{
    // Set sstatus.SIE to 1
    asm("csrsi sstatus, (1 << 1)");
}

void enable_timer_interrupt()
{
    // Set sie.TSIE to 1
    asm("li t0, (1 << 5);"
        "csrs sie, t0;");
}

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");

    // enable_interrupt();
    // enable_timer_interrupt();
    // sbi_set_timer(10000000);

    // switch_to_user_mode();
    // asm("ecall");

    uart_init();

    run_shell();
    return 0;
}
