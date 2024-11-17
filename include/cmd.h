#pragma once

#include "list.h"

struct command {
    const char *name;
    const char *help;
    void (*func)(char *);
    struct list_head list;
};

void cmd_init();
void register_command(const char *name, const char *help, void (*func)(char *));
int exec_command(char *input);

void cmd_help();
void cmd_hello();
void cmd_reboot();
void cmd_clear();
void cmd_ls();
void cmd_cat(char *args);
void cmd_exec(char *args);
