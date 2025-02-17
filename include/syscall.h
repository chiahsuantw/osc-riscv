#pragma once

#include "traps.h"

long sys_getpid();
long sys_read(char *buf, long count);
long sys_write(const char *buf, long count);
long sys_exec(const char *filename, const char *const *argv);
long sys_fork(struct pt_regs *regs);
long sys_exit(int error_code);
long sys_kill(long pid);
long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
              unsigned long flags);
