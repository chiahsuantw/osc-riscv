#pragma once

#include "list.h"

#define STACK_SIZE 0x2000

struct thread_struct {
    unsigned long ra;
    unsigned long sp;    /* Kernel mode stack */
    unsigned long s[12]; /* s[0]: frame pointer */
};

enum task_state {
    TASK_RUNNING,
    TASK_ZOMBIE,
};

struct task_struct {
    struct thread_struct context;
    long pid;
    enum task_state state;
    long kernel_sp;
    long user_sp;
    struct list_head list;
};

struct task_struct *get_current();
struct task_struct *find_task(int pid);
void schedule();
void idle();
void kthread_init();
struct task_struct *kthread_create(void (*threadfn)());
void kthread_stop(struct task_struct *task);
void kthread_exit();