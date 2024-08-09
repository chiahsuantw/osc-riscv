#include "cmd.h"
#include "sbi.h"
#include "string.h"
#include "uart.h"

struct command commands[] = {
    { .name = "help", .help = "print the help menu", .func = help },
    { .name = "hello", .help = "print \"Hello World!\"", .func = hello },
    { .name = "reboot", .help = "reboot the device", .func = reboot },
    { .name = "NULL" } // Must put a NULL command at the end!
};

void help()
{
    int i = 0;
    while (1) {
        if (!strcmp(commands[i].name, "NULL"))
            break;
        uart_puts(commands[i].name);
        uart_puts("\t: ");
        uart_puts(commands[i].help);
        uart_putc('\n');
        i++;
    }
}

void hello()
{
    uart_puts("Hello, World!\n");
}

void reboot()
{
    sbi_system_reset(SBI_SRST_TYPE_WARM_REBOOT, SBI_SRST_REASON_NONE);
}
