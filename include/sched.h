#pragma once

#include "list.h"
#include "signal.h"
#include "vm.h"

#define STACK_SIZE 0x2000

struct thread_struct {
    unsigned long ra;
    unsigned long sp;    /* Kernel mode stack */
    unsigned long s[12]; /* s[0]: frame pointer */
};

struct thread_info {
    long kernel_sp;
    long user_sp;
};

enum task_state {
    TASK_RUNNING,
    TASK_ZOMBIE,
};

struct task_struct {
    struct thread_struct thread;
    struct thread_info thread_info;
    long pid;
    enum task_state state;
    unsigned long kernel_stack;
    unsigned long user_stack;
    struct mm_struct mm;
    void (*sighand[_NSIG])();
    sigset_t blocked;
    sigset_t pending;
    struct list_head list;
};

struct task_struct *get_current();
struct task_struct *find_task_by_pid(int pid);
void schedule();
void idle();
void kthread_init();
struct task_struct *kthread_create(void (*threadfn)());
void kthread_stop(struct task_struct *task);
void kthread_exit();
