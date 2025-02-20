#pragma once

#include "list.h"
#include "traps.h"

#define _NSIG 32

#define SIG_DFL 0

typedef unsigned long sigset_t;

struct sigframe {
    int signo;
    struct pt_regs ucontext;
};

long sys_signal(int sig, void (*handler)());
long sys_sigreturn(struct pt_regs *regs);
long sys_kill(long pid, int sig);
void do_signal(struct pt_regs *regs);
