#include "vm.h"
#include "list.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "string.h"
#include "traps.h"

static void pagewalk(struct mm_struct *mm, unsigned long va, unsigned long pa,
                     unsigned long prot)
{
    unsigned long *pte = (unsigned long *)mm->pgd;
    for (int level = 2; level >= 0; level--) {
        unsigned long idx = (va >> (12 + 9 * level)) & 0x1ff;
        if (level == 0) {
            pte[idx] = pa >> 2 | prot;
            return;
        }
        if (pte[idx] == 0) {
            unsigned long *page_addr = (unsigned long *)kmalloc(PAGE_SIZE);
            memset(page_addr, 0, PAGE_SIZE);
            pte[idx] = virt_to_phys(page_addr) >> 2 | PAGE_PRESENT;
        }
        pte = (unsigned long *)phys_to_virt((pte[idx] & ~0x3ff) << 2);
    }
}

void map_pages(struct mm_struct *mm, unsigned long va, unsigned long size,
               unsigned long pa, unsigned long prot)
{
    // FIXME: Check if the virtual and physical addresses are page-aligned
    for (int i = 0; i < size; i += PAGE_SIZE)
        pagewalk(mm, va + i, pa + i, prot);
}

void vm_mmap(struct mm_struct *mm, unsigned long file, unsigned long addr,
             unsigned long len, unsigned long prot, unsigned long flags)
{
    struct vm_area_struct *vma = kmalloc(sizeof(struct vm_area_struct));
    vma->vm_start = addr;
    vma->vm_end = addr + len;
    vma->vm_page_prot = prot;
    vma->vm_file = file;
    list_add_tail(&vma->list, &mm->mmap);
}

void do_page_fault(struct pt_regs *regs)
{
    struct vm_area_struct *target_vma = 0;
    struct list_head *pos;
    list_for_each(pos, &get_current()->mm.mmap) {
        struct vm_area_struct *vma =
            list_entry(pos, struct vm_area_struct, list);
        if (vma->vm_start <= regs->badaddr && regs->badaddr < vma->vm_end) {
            target_vma = vma;
            break;
        }
    }

    if (target_vma) {
        struct mm_struct *mm = &get_current()->mm;
        printk("Page fault at %p\n", regs->badaddr);
        unsigned long va = regs->badaddr & ~(PAGE_SIZE - 1);
        unsigned long prot = PAGE_BASE;
        if (target_vma->vm_page_prot & PROT_READ)
            prot |= PAGE_READ;
        if (target_vma->vm_page_prot & PROT_WRITE)
            prot |= PAGE_WRITE;
        if (target_vma->vm_page_prot & PROT_EXEC)
            prot |= PAGE_EXEC;
        if (target_vma->vm_file) {
            unsigned long offset = va - target_vma->vm_start;
            unsigned long pa = virt_to_phys(target_vma->vm_file + offset);
            map_pages(mm, va, PAGE_SIZE, pa, prot);
        } else {
            unsigned long pa = virt_to_phys(kmalloc(PAGE_SIZE));
            map_pages(mm, va, PAGE_SIZE, pa, prot);
        }
    } else {
        printk("Segmentation fault\n");
        kthread_exit();
    }
}
