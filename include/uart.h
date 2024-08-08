#pragma once

#define UART_BASE 0x10000000UL

#ifdef __QEMU__
#define UART_RBR (unsigned char *)(UART_BASE + 0x0)
#define UART_THR (unsigned char *)(UART_BASE + 0x0)
#define UART_LSR (unsigned char *)(UART_BASE + 0x5)
#else
#define UART_RBR (unsigned int *)(UART_BASE + 0x0)
#define UART_THR (unsigned int *)(UART_BASE + 0x0)
#define UART_LSR (unsigned int *)(UART_BASE + 0x14)
#endif

char uart_getc();
void uart_putc(char c);
void uart_puts(const char *s);
void uart_hex(unsigned int h);
