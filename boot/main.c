#define UART_BASE 0x10000000UL

// #define UART_RBR (unsigned int *)(UART_BASE + 0x0)
// #define UART_THR (unsigned int *)(UART_BASE + 0x0)
// #define UART_LSR (unsigned int *)(UART_BASE + 0x14)

#define UART_RBR (unsigned char *)(UART_BASE + 0x0)
#define UART_THR (unsigned char *)(UART_BASE + 0x0)
#define UART_LSR (unsigned char *)(UART_BASE + 0x5)

char uart_recv();
void uart_putc(char c);
void uart_puts(const char *s);
int atoi(const char *s);

int main()
{
    uart_puts("\nUART Bootloader\n");

    // Get the kernel image size
    char buf[16] = { 0 };
    for (int i = 0; i < 16; i++) {
        buf[i] = uart_recv();
        if (buf[i] == '\n') {
            buf[i] = '\0';
            break;
        }
    }

    uart_puts("Kernel size: ");
    uart_puts(buf);
    uart_puts(" bytes\n");

    // Load the kernel image
    int size = atoi(buf);
    char *kernel = (char *)0x80200000;
    while (size--)
        *kernel++ = uart_recv();

    uart_puts("Kernel loaded\n");

    // Jump to the kernel
    asm("li ra, 0x80200000; ret;");
    return 0;
}

char uart_recv()
{
    while ((*UART_LSR & 0x01) == 0)
        ;
    return (char)*UART_RBR;
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

int atoi(const char *s)
{
    int ret = 0;
    int i = 0;

    while (s[i] == ' ')
        i++;

    while (s[i] >= '0' && s[i] <= '9') {
        ret = ret * 10 + (s[i] - '0');
        i++;
    }

    return ret;
}
