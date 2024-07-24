#pragma once

#define UART_BASE 0x10000000UL

#define UART_RBR (unsigned int *)(UART_BASE + 0x0)
#define UART_THR (unsigned int *)(UART_BASE + 0x0)
#define UART_LSR (unsigned int *)(UART_BASE + 0x14)

char uart_getc();
void uart_putc(char c);
void uart_puts(const char *s);
