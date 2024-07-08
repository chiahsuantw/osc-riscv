#include "uart.h"

int start_kernel()
{
    uart_puts("NYCU OSC RISC-V KERNEL\n");
    return 0;
}
