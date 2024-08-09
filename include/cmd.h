#pragma once

struct command {
    const char *name;
    const char *help;
    void (*func)(void);
    // TODO: Add function arguments
};

extern struct command commands[];

void help();
void hello();
void reboot();
