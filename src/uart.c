#include "uart.h"

void uart_putc(char c)
{
    // while ((*UART_LSR & 0x40) == 0)
    //     ;
    // while (((*UART_LSR >> 5) & 1) != 1)
    //     ;
    *UART_THR = c;
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}
