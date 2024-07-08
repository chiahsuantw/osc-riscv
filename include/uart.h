#pragma once

#define UART_BASE 0x10000000
#define UART_THR  (unsigned int *)(UART_BASE + 0x0)
#define UART_LSR  (unsigned int *)(UART_BASE + 0x14)

void uart_putc(char c);
void uart_puts(const char *s);
