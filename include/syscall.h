#pragma once

#include "traps.h"

#define SYS_GETPID     0
#define SYS_UART_READ  1
#define SYS_UART_WRITE 2
#define SYS_EXEC       3
#define SYS_FORK       4
#define SYS_EXIT       5
#define SYS_KILL       6
#define SYS_SIGNAL     7
#define SYS_SIGRETURN  8
#define SYS_SIG_KILL   9
#define SYS_MMAP       10
#define SYS_OPEN       11
#define SYS_CLOSE      12
#define SYS_READ       13
#define SYS_WRITE      14
#define SYS_MKDIR      15
#define SYS_MOUNT      16
#define SYS_CHDIR      17
#define SYS_LSEEK      18
#define SYS_IOCTL      19

long sys_getpid();
long sys_uart_read(char *buf, long count);
long sys_uart_write(const char *buf, long count);
long sys_exec(const char *filename, const char *const *argv);
long sys_fork(struct pt_regs *regs);
long sys_exit(int error_code);
// long sys_kill(long pid);
long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
              unsigned long flags);
long sys_open(const char *pathname, int flags);
long sys_close(int fd);
long sys_read(int fd, char *buf, long count);
long sys_write(int fd, const char *buf, long count);
long sys_mkdir(const char *pathname);
long sys_mount(const char *source, const char *target, const char *filesystem);
long sys_chdir(const char *path);
// long sys_lseek(int fd, long offset, int whence);
