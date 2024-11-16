#include "printk.h"
#include "string.h"
#include "uart.h"
#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void printint(long long xx, int base, int sign)
{
    char buf[16] = { 0 };
    unsigned long long x = (sign && (sign = (xx < 0))) ? -xx : xx;

    int i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        uart_putc(buf[i]);
}

static void printptr(unsigned long x)
{
    uart_puts("0x");
    for (int i = 0; i < sizeof(unsigned long) * 2; i++, x <<= 4)
        uart_putc(digits[x >> (sizeof(unsigned long) * 8 - 4)]);
}

int vprintk(const char *fmt, va_list args)
{
    while (*fmt) {
        if (*fmt != '%') {
            uart_putc(*fmt++);
            continue;
        }
        fmt++;
        if (!memcmp(fmt, "d", 1)) {
            printint(va_arg(args, int), 10, 1);
            fmt++;
        } else if (!memcmp(fmt, "ld", 2)) {
            printint(va_arg(args, unsigned long), 10, 1);
            fmt += 2;
        } else if (!memcmp(fmt, "u", 1)) {
            printint(va_arg(args, int), 10, 0);
            fmt++;
        } else if (!memcmp(fmt, "lu", 2)) {
            printint(va_arg(args, unsigned long), 10, 0);
            fmt += 2;
        } else if (!memcmp(fmt, "x", 1)) {
            printint(va_arg(args, int), 16, 0);
            fmt++;
        } else if (!memcmp(fmt, "lx", 2)) {
            printint(va_arg(args, unsigned long), 16, 0);
            fmt += 2;
        } else if (!memcmp(fmt, "c", 1)) {
            uart_putc(va_arg(args, int));
            fmt++;
        } else if (!memcmp(fmt, "s", 1)) {
            char *s = va_arg(args, char *);
            if (s == 0)
                s = "(null)";
            uart_puts(s);
            fmt++;
        } else if (!memcmp(fmt, "p", 1)) {
            printptr(va_arg(args, unsigned long));
            fmt++;
        } else if (!memcmp(fmt, "%%", 1)) {
            uart_putc('%');
        }
    }
    return 0;
}

int printk(const char *fmt, ...)
{
    va_list args;
    int r;
    va_start(args, fmt);
    r = vprintk(fmt, args);
    va_end(args);
    return r;
}
