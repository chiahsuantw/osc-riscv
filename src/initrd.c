#include "initrd.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "string.h"
#include "utils.h"
#include "vm.h"

#ifdef __QEMU__
#define INITRD_PHYS_BASE 0xA0200000
#else
#define INITRD_PHYS_BASE 0x46100000
#endif

void *INITRD_BASE = (void *)phys_to_virt(INITRD_PHYS_BASE);

void *initrd_get_next_hdr(const void *ptr)
{
    if (!memcmp(ptr + sizeof(struct cpio_t), "TRAILER!!!", 10))
        return 0;
    struct cpio_t *hdr = (struct cpio_t *)ptr;
    int namesize = hextoi(hdr->namesize, 8);
    int filesize = hextoi(hdr->filesize, 8);
    int headsize = align(sizeof(struct cpio_t) + namesize, 4);
    int datasize = align(filesize, 4);
    return (void *)(ptr + headsize + datasize);
}

void initrd_list()
{
    const char *ptr = INITRD_BASE;
    while (initrd_get_next_hdr(ptr)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;
        int namesize = hextoi(hdr->namesize, 8);
        char pathname[namesize];
        strncpy(pathname, ptr + sizeof(struct cpio_t), namesize);
        printk("%s\n", pathname);
        ptr = initrd_get_next_hdr(ptr);
    }
}

void initrd_cat(const char *target)
{
    if (target == 0)
        return;
    const char *ptr = INITRD_BASE;
    while (initrd_get_next_hdr(ptr)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;
        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);
        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        if (!memcmp(ptr + sizeof(struct cpio_t), target, namesize)) {
            char data[filesize + 1];
            memset(data, 0, filesize + 1);
            strncpy(data, ptr + headsize, filesize);
            printk("%s\n", data);
            return;
        }
        ptr = initrd_get_next_hdr(ptr);
    }
    printk("cat: %s not found\n", target);
}

void initrd_exec(const char *target)
{
    if (target == 0)
        return;
    const char *ptr = INITRD_BASE;
    while (initrd_get_next_hdr(ptr)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;
        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);
        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        if (!memcmp(ptr + sizeof(struct cpio_t), target, namesize)) {
            void *program = kmalloc(align(filesize, PAGE_SIZE));
            memcpy(program, ptr + headsize, filesize);
            struct task_struct *task = kthread_create(program);

            map_pages(&task->mm, 0x0, align(filesize, PAGE_SIZE),
                      virt_to_phys(program), PAGE_RX);
            map_pages(&task->mm, 0x3fffffb000, PAGE_SIZE * 4,
                      virt_to_phys(task->user_stack), PAGE_RW);

            asm("sfence.vma");
            asm("csrw satp, %0" ::"r"((unsigned long)0x8 << 60 |
                                      virt_to_phys(task->mm.pgd) >> 12));
            asm("sfence.vma");

            asm("csrw sscratch, %0" ::"r"(task));
            asm("mv sp, %0" ::"r"(0x3ffffff000));
            asm("csrw sepc, %0" ::"r"(0x0));
            asm("li t0, (1 << 8);"
                "csrc sstatus, t0;");
            asm("sret");
        }
        ptr = initrd_get_next_hdr(ptr);
    }
    printk("cat: %s not found\n", target);
}
