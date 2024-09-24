#include "uart.h"

void uart_init()
{
    // Enable RX interrupt
    *UART_IER = 0x1;

#define PLIC_BASE             0x0C000000L
#define PLIC_S_ENABLE(hart)   (PLIC_BASE + 0x2080 + (hart) * 0x100)
#define PLIC_S_PRIORITY(hart) (PLIC_BASE + 0x201000 + (hart) * 0x2000)
#define UART0_IRQ             10

    int hartid = 0;
    // Set UART interrupt priority
    *(unsigned int *)(PLIC_BASE + UART0_IRQ * 4) = 1;
    // Enable PLIC UART interrupt
    *(unsigned int *)PLIC_S_ENABLE(hartid) = (1 << UART0_IRQ);
    // Set priority threshold to 0
    *(unsigned int *)PLIC_S_PRIORITY(hartid) = 0;

    // Enable external interrupt
    asm("li t0, (1 << 9);"
        "csrs sie, t0;");

    // Enable interrupt
    asm("csrsi sstatus, (1 << 1)");
}

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
