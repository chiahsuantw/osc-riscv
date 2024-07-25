#include "uart.h"

char uart_getc()
{
    while ((*UART_LSR & 0x01) == 0)
        ;
    char c = (char)*UART_RBR;
    return c == '\r' ? '\n' : c;
}

void uart_putc(char c)
{
    if (c == '\n')
        uart_putc('\r');

    while ((*UART_LSR & 0x20) == 0)
        ;
    *UART_THR = c;
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

void uart_hex(unsigned int h)
{
    uart_puts("0x");
    unsigned int n;
    for (int c = 28; c >= 0; c -= 4) {
        n = (h >> c) & 0xf;
        n += n > 9 ? 0x37 : '0';
        uart_putc(n);
    }
}
