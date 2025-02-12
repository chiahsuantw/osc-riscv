#pragma once

#include "list.h"

#ifdef __QEMU__
#define TIME_FREQ 10000000
#else
#define TIME_FREQ 4000000
#endif

struct timer {
    void (*func)(void *);
    void *arg;
    long time;
    struct list_head list;
};

void timer_init();
void enable_timer_interrupt();
void disable_timer_interrupt();
void timer_irq_handler();
long get_uptime();
void set_timeout(const char *message, int after);
