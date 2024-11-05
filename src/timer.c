#include "timer.h"

void timer_init()
{
    enable_timer_interrupt();
}

void enable_timer_interrupt()
{
    // Set sie.TSIE to 1
    asm("li t0, (1 << 5);"
        "csrs sie, t0;");
}

void disable_timer_interrupt()
{
    // Set sie.TSIE to 0
    asm("li t0, (1 << 5);"
        "csrc sie, t0;");
}

void timer_irq_handler()
{
}

unsigned long get_uptime()
{
    return 0;
}

void set_timeout(const char *message, int after)
{
}
