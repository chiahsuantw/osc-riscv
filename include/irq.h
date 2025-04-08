#pragma once

#include "list.h"
#include "traps.h"

#define RISCV_XLEN 64

/* Interrupt causes (minus the high bit) */
#define IRQ_CAUSE(x) ((x) & ~(1UL << (RISCV_XLEN - 1)))
#define IRQ_S_TIMER  5
#define IRQ_S_EXT    9

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
