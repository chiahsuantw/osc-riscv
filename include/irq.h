#pragma once

#include "list.h"

struct irq_task {
    void (*handler)();
    int priority;
    int running;
    struct list_head list;
};

void irq_init();
void enable_interrupt();
void disable_interrupt();
void switch_to_user_mode();
void irq_entry();
