#include "sched.h"
#include "mm.h"
#include "string.h"
#include "vm.h"

static int nr_threads = 0;
static struct list_head runqueue;

extern void switch_to(struct task_struct *prev, struct task_struct *next);

struct task_struct *get_current()
{
    struct task_struct *current;
    asm volatile("mv %0, tp" : "=r"(current));
    return current;
}

struct task_struct *find_task(int pid)
{
    struct list_head *pos;
    list_for_each(pos, &runqueue) {
        struct task_struct *task = list_entry(pos, struct task_struct, list);
        if (task->pid == pid)
            return task;
    }
    return 0;
}

void schedule()
{
    struct task_struct *current = get_current();
    switch_to(current, list_next_entry_circular(current, &runqueue, list));
}

void kill_zombies()
{
    struct list_head *pos, *tmp;
    list_for_each_safe(pos, tmp, &runqueue) {
        struct task_struct *task = list_entry(pos, struct task_struct, list);
        if (task->state == TASK_ZOMBIE) {
            list_del_init(&task->list);
            kfree((void *)task->kernel_stack);
            kfree((void *)task->user_stack);
            kfree(task);
        }
    }
}

void idle()
{
    // TODO: Can we remove the loop?
    while (1) {
        kill_zombies();
        schedule();
    }
}

void kthread_init()
{
    INIT_LIST_HEAD(&runqueue);
    struct task_struct *init = kthread_create(idle);
    asm volatile("mv tp, %0" : : "r"(init));
}

struct task_struct *kthread_create(void (*threadfn)())
{
    struct task_struct *task = kmalloc(sizeof(struct task_struct));
    task->pid = nr_threads++;
    task->state = TASK_RUNNING;
    task->kernel_stack = (unsigned long)kmalloc(STACK_SIZE);
    task->user_stack = (unsigned long)kmalloc(STACK_SIZE);
    task->kernel_sp = task->kernel_stack + STACK_SIZE;
    task->user_sp = task->user_stack + STACK_SIZE;
    task->context.ra = (unsigned long)threadfn;
    task->context.sp = task->kernel_sp;
    task->pgd = kmalloc(PAGE_SIZE);
    memcpy(task->pgd, (const void *)phys_to_virt(0x80100000), PAGE_SIZE);
    list_add_tail(&task->list, &runqueue);
    return task;
}

void kthread_stop(struct task_struct *task)
{
    task->state = TASK_ZOMBIE;
    schedule();
}

void kthread_exit()
{
    get_current()->state = TASK_ZOMBIE;
    schedule();
}
