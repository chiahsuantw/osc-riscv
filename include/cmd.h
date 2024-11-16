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

void help();
void hello();
void reboot();
void clear();
