
#include "irq.h"
#include "mm.h"
#include "timer.h"

static struct list_head irq_tasks;

void irq_init()
{
    INIT_LIST_HEAD(&irq_tasks);
}

void enable_interrupt()
{
    // Set sstatus.SIE to 1
    asm("csrsi sstatus, (1 << 1)");
}

void disable_interrupt()
{
    // Set sstatus.SIE to 0
    asm("csrci sstatus, (1 << 1)");
}

void switch_to_user_mode()
{
    asm("csrw sepc, ra;");
    asm("sret");
}

static void irq_add_task(void (*callback)(), int priority)
{
    struct irq_task *task = (struct irq_task *)kmalloc(sizeof(struct irq_task));
    task->handler = callback;
    task->priority = priority;
    task->running = 0;
    INIT_LIST_HEAD(&task->list);

    struct list_head *pos;
    list_for_each(pos, &irq_tasks) {
        struct irq_task *curr = list_entry(pos, struct irq_task, list);
        if (curr->priority > task->priority) {
            list_add_tail(&task->list, pos);
            return;
        }
    }
    list_add_tail(&task->list, &irq_tasks);
}

void do_irq(struct pt_regs *regs)
{
    printk("do_irq\n");
    disable_interrupt();
    if (0) {        // UART interrupt
    } else if (1) { // Timer interrupt
        disable_timer_interrupt();
        irq_add_task(timer_irq_handler, 1);
    }
    enable_interrupt();

    while (!list_empty(&irq_tasks)) {
        struct irq_task *head =
            list_first_entry(&irq_tasks, struct irq_task, list);
        if (head->running)
            break;

        disable_interrupt();
        head->running = 1;
        enable_interrupt();

        head->handler();

        disable_interrupt();
        list_del_init(&head->list);
        kfree(head);
        enable_interrupt();
    }
}
