#include "shell.h"
#include "uart.h"

int start_kernel()
{
    uart_puts("\nNYCU OSC RISC-V KERNEL\n");
    run_shell();
    return 0;
}
