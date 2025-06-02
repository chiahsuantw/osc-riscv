#include "sched.h"
#include "list.h"
#include "mm.h"
#include "signal.h"
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

struct task_struct *find_task_by_pid(int pid)
{
    struct list_head *pos;
    list_for_each(pos, &runqueue) {
        struct task_struct *task = list_entry(pos, struct task_struct, list);
        if (task->pid == pid)
            return task;
    }
    return 0;
}

void switch_mm(unsigned long pgd, unsigned long satp_mode)
{
    asm("sfence.vma");
    asm("csrw satp, %0" ::"r"(satp_mode << 60 | virt_to_phys(pgd) >> 12));
    asm("sfence.vma");
}

void schedule()
{
    struct task_struct *current = get_current();
    struct task_struct *next =
        list_next_entry_circular(current, &runqueue, list);
    if (next == current)
        return;
    switch_mm((unsigned long)next->mm.pgd, 0x8);
    switch_to(current, next);
}

void kill_zombies()
{
    struct list_head *pos, *tmp;
    list_for_each_safe(pos, tmp, &runqueue) {
        struct task_struct *task = list_entry(pos, struct task_struct, list);
        if (task->state == TASK_ZOMBIE) {
            list_del_init(&task->list);
            // kfree((void *)task->stack);
            // kfree((void *)task->user_stack);
            // kfree(task);
        }
    }
}

void idle()
{
    while (1) {
        kill_zombies();
        schedule();
    }
}

void kthread_init()
{
    INIT_LIST_HEAD(&runqueue);
    struct task_struct *tidle = kthread_create(idle);
    asm volatile("mv tp, %0" : : "r"(tidle));
}

struct task_struct *kthread_create(void (*threadfn)())
{
    struct task_struct *task = kmalloc(sizeof(struct task_struct));
    task->pid = nr_threads++;
    task->state = TASK_RUNNING;
    task->stack = (unsigned long)kmalloc(STACK_SIZE);
    task->thread.ra = (unsigned long)threadfn;
    task->thread.sp = task->stack + STACK_SIZE;
    task->thread_info.kernel_sp = task->thread.sp;
    INIT_LIST_HEAD(&task->mm.mmap);
    task->mm.pgd = kmalloc(PAGE_SIZE);
    memset(task->cwd, 0, PATH_MAX);
    strncpy(task->cwd, "/", 1);
    memset(task->fdt, 0, sizeof(task->fdt));
    memcpy(task->mm.pgd, (const void *)phys_to_virt(PGD_BASE), PAGE_SIZE);
    memset(task->sighand, SIG_DFL, sizeof(task->sighand));
    task->blocked = 0;
    task->pending = 0;
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
