#pragma once

#define SBI_SRST_RESET_TYPE_SHUTDOWN    0
#define SBI_SRST_RESET_TYPE_COLD_REBOOT 1
#define SBI_SRST_RESET_TYPE_WARM_REBOOT 2
#define SBI_SRST_RESET_REASON_NONE      0

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
                        unsigned long arg1, unsigned long arg2,
                        unsigned long arg3, unsigned long arg4,
                        unsigned long arg5);

void sbi_console_putchar(int ch);
int sbi_console_getchar();

struct sbiret sbi_system_reset(unsigned int type, unsigned int reason);
