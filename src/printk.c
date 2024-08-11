#include "printk.h"
#include "string.h"
#include "uart.h"
#include <stdarg.h>

/*
 * `d` - signed decimal integer
 * `u` - unsigned decimal integer
 * `x` - unsigned hexadecimal integer
 * `c` - character
 * `s` - string
 * `p` - pointer
 * `%` - %
 */
int vprintk(const char *fmt, va_list args)
{
    while (*fmt) {
        if (*fmt != '%') {
            uart_putc(*fmt++);
            continue;
        }
        fmt++;
        if (!memcmp(fmt, "d", 1)) {
            fmt++;
        } else if (!memcmp(fmt, "ld", 2)) {
            fmt += 2;
        } else if (!memcmp(fmt, "u", 1)) {
            fmt++;
        } else if (!memcmp(fmt, "lu", 2)) {
            fmt += 2;
        } else if (!memcmp(fmt, "x", 1)) {
            fmt++;
        } else if (!memcmp(fmt, "lx", 2)) {
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
