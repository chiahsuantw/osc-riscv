#include "uart.h"

int start_kernel()
{
    uart_puts("\nNYCU OSC RISC-V KERNEL\n");

    while (1) {
        uart_putc(uart_getc());
    }

    return 0;
}
