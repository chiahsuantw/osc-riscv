#pragma once

#define SBI_SRST_TYPE_WARM_REBOOT 2
#define SBI_SRST_REASON_NONE      0

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
int sbi_probe_extension(int extid);
struct sbiret sbi_set_timer(unsigned long stime_value);
struct sbiret sbi_system_reset(unsigned int type, unsigned int reason);
int sbi_debug_console_write(const char *bytes, unsigned int size);
int sbi_debug_console_read(char *bytes, unsigned int size);
