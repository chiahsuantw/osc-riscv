#pragma once

void timer_init();
void enable_timer_interrupt();
void disable_timer_interrupt();
void timer_irq_handler();
unsigned long get_uptime();
void set_timeout(const char *message, int after);
