#include "cmd.h"
#include "printk.h"
#include "sbi.h"
#include "string.h"

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
        printk("%s\t: %s\n", commands[i].name, commands[i].help);
        i++;
    }
}

void hello()
{
    printk("Hello, World!\n");
}

void reboot()
{
    sbi_system_reset(SBI_SRST_TYPE_WARM_REBOOT, SBI_SRST_REASON_NONE);
}
