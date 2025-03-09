/* Fork system call test */

#include "printk.h"
#include "sched.h"

static long getpid()
{
    long ret = -1;
    asm("li a7, 0");
    asm("ecall");
    asm("mv %0, a0" : "=r"(ret));
    return ret;
}

static int fork()
{
    int ret = -1;
    asm("li a7, 4");
    asm("ecall");
    asm("mv %0, a0" : "=r"(ret));
    return ret;
}

static void exit(int status)
{
    asm("mv a0, %0" : : "r"(status));
    asm("li a7, 5");
    asm("ecall");
}

void do_fork_test()
{
    printk("Fork test (pid = %d)\n", getpid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) {
        long cur_sp;
        asm("mv %0, sp" : "=r"(cur_sp));
        printk("child1: pid = %d, cnt = %d, ptr = %p, sp = %p\n", getpid(), cnt,
               &cnt, cur_sp);
        cnt++;

        if ((ret = fork()) != 0) {
            asm("mv %0, sp" : "=r"(cur_sp));
            printk("child1: pid = %d, cnt = %d, ptr = %p, sp = %p\n", getpid(),
                   cnt, &cnt, cur_sp);
            cnt++;
        } else {
            while (cnt < 5) {
                asm("mv %0, sp" : "=r"(cur_sp));
                printk("child2: pid = %d, cnt = %d, ptr = %p, sp = %p\n",
                       getpid(), cnt, &cnt, cur_sp);
                for (int i = 0; i < 1000000000; i++)
                    ;
                cnt++;
            }
        }
    } else {
        printk("parent: pid = %d, child pid = %d\n", getpid(), ret);
    }
    exit(0);
}

void fork_test()
{
    // Switch to the user mode
    asm("csrw sscratch, %0" ::"r"(get_current()));
    asm("mv sp, %0" ::"r"(get_current()->thread_info.user_sp));
    asm("csrw sepc, %0" ::"r"(do_fork_test));
    asm("li t0, (1 << 8);"
        "csrc sstatus, t0;");
    asm("sret");
}
