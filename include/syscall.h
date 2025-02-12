#pragma once

#include "traps.h"

long sys_getpid();
long sys_read(char *buf, long count);
long sys_write(const char *buf, long count);
int sys_exec(const char *pathname, const char *const *argv);
long sys_fork(struct pt_regs *regs);
void sys_exit(int status);
int sys_kill(long pid);
long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
              unsigned long flags);
