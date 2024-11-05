#pragma once

#include "list.h"

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
