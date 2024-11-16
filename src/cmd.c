#include "cmd.h"
#include "mm.h"
#include "printk.h"
#include "sbi.h"
#include "string.h"

static struct list_head commands;

void cmd_init()
{
    INIT_LIST_HEAD(&commands);
    register_command("help", "Show all commands", help);
    register_command("hello", "Show hello message", hello);
    register_command("reboot", "Reboot the device", reboot);
    register_command("clear", "Clear the screen", clear);
}

void register_command(const char *name, const char *help, void (*func)(char *))
{
    struct command *cmd = (struct command *)kmalloc(sizeof(struct command));
    cmd->name = name;
    cmd->help = help;
    cmd->func = func;
    list_add_tail(&cmd->list, &commands);
}

static int isspace(int c)
{
    return c == ' ';
}

int exec_command(char *input)
{
    while (isspace(*input))
        input++;
    char *end = input + strlen(input) - 1;
    while (end > input && isspace(*end))
        *end-- = 0;

    if (strlen(input) == 0)
        return -1;

    char *args = strchr(input, ' ');
    while (args && isspace(*args))
        args++;
    char *command = strtok(input, " ");

    struct list_head *pos;
    list_for_each(pos, &commands) {
        struct command *cmd = list_entry(pos, struct command, list);
        if (!strcmp(cmd->name, command)) {
            cmd->func(args);
            return 0;
        }
    }
    printk("%s: command not found\n", command);
    return -1;
}

void help()
{
    struct list_head *pos;
    list_for_each(pos, &commands) {
        struct command *cmd = list_entry(pos, struct command, list);
        printk("%s\t- %s\n", cmd->name, cmd->help);
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

void clear()
{
    printk("\033[H\033[J");
}