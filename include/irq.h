#pragma once

#include "list.h"
#include "traps.h"

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
void do_irq(struct pt_regs *regs);
