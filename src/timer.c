#include "timer.h"
#include "mm.h"
#include "sbi.h"
#include "uart.h"

static struct list_head timer_queue;

void timer_init()
{
    INIT_LIST_HEAD(&timer_queue);
    enable_timer_interrupt();
    sbi_set_timer(10000000); // TODO: Set the first timer interrupt
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
    // Clear sip.STIP
    asm("li t0, (1 << 5);"
        "csrc sip, t0;");

    // Set up the next timer interrupt
    unsigned long ticks;
    asm("rdtime %0" : "=r"(ticks));
    ticks += 10000000;
    sbi_set_timer(ticks);

    // Check the timer queue
    while (!list_empty(&timer_queue)) {
        struct timer *timer =
            list_first_entry(&timer_queue, struct timer, list);
        if (timer->time > get_uptime())
            break;
        timer->func(timer->arg);
        list_del_init(&timer->list);
        kfree(timer);
    }
}

long get_uptime()
{
    // TODO: Get the timer frequency
    unsigned long ticks, freq = 3686400;
    asm("rdtime %0" : "=r"(ticks));
    return ticks / freq;
}

static void timer_add(void (*callback)(void *), void *arg, int after)
{
    struct timer *timer = (struct timer *)kmalloc(sizeof(struct timer));
    timer->func = callback;
    timer->arg = arg;
    timer->time = get_uptime() + after;
    INIT_LIST_HEAD(&timer->list);

    // Enqueue the timer
    struct list_head *pos;
    list_for_each(pos, &timer_queue) {
        struct timer *curr = list_entry(pos, struct timer, list);
        if (curr->time > timer->time) {
            list_add_tail(&timer->list, pos);
            return;
        }
    }
    list_add_tail(&timer->list, &timer_queue);
}

void set_timeout(const char *message, int after)
{
    timer_add((void (*)(void *))uart_puts, (void *)message, after);
}
