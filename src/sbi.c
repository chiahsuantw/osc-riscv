#include "sbi.h"

#define SBI_EXT_PUTCHAR    0x1
#define SBI_EXT_GETCHAR    0x2
#define SBI_EXT_BASE       0x10
#define SBI_EXT_BASE_PROBE 3
#define SBI_EXT_TIME       0x54494D45
#define SBI_EXT_TIME_SET   0
#define SBI_EXT_SRST       0x53525354
#define SBI_EXT_SRST_RST   0
#define SBI_EXT_DBCN       0x4442434E
#define SBI_EXT_DBCN_WRITE 0
#define SBI_EXT_DBCN_READ  1

struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
                        unsigned long arg1, unsigned long arg2,
                        unsigned long arg3, unsigned long arg4,
                        unsigned long arg5)
{
    struct sbiret ret;

    register unsigned long a0 asm("a0") = (unsigned long)arg0;
    register unsigned long a1 asm("a1") = (unsigned long)arg1;
    register unsigned long a2 asm("a2") = (unsigned long)arg2;
    register unsigned long a3 asm("a3") = (unsigned long)arg3;
    register unsigned long a4 asm("a4") = (unsigned long)arg4;
    register unsigned long a5 asm("a5") = (unsigned long)arg5;
    register unsigned long a6 asm("a6") = (unsigned long)fid;
    register unsigned long a7 asm("a7") = (unsigned long)ext;
    asm volatile("ecall"
                 : "+r"(a0), "+r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");
    ret.error = a0;
    ret.value = a1;

    return ret;
}

void sbi_console_putchar(int ch)
{
    sbi_ecall(SBI_EXT_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
}

int sbi_console_getchar()
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_GETCHAR, 0, 0, 0, 0, 0, 0, 0);
    return ret.error;
}

int sbi_probe_extension(int extid)
{
    struct sbiret ret;

    ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE, extid, 0, 0, 0, 0, 0);
    if (!ret.error)
        if (ret.value)
            return ret.value;

    return 0;
}

struct sbiret sbi_set_timer(unsigned long stime_value)
{
    return sbi_ecall(SBI_EXT_TIME, SBI_EXT_TIME_SET, stime_value, 0, 0, 0, 0,
                     0);
}

struct sbiret sbi_system_reset(unsigned int type, unsigned int reason)
{
    return sbi_ecall(SBI_EXT_SRST, SBI_EXT_SRST_RST, type, reason, 0, 0, 0, 0);
}

int sbi_debug_console_write(const char *bytes, unsigned int size)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_DBCN, SBI_EXT_DBCN_WRITE, size,
                    (unsigned long)bytes, 0, 0, 0, 0);
    return ret.error ? ret.error : ret.value;
}

int sbi_debug_console_read(char *bytes, unsigned int size)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_DBCN, SBI_EXT_DBCN_READ, size, (unsigned long)bytes,
                    0, 0, 0, 0);
    return ret.error ? ret.error : ret.value;
}
