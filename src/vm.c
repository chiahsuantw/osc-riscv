#include "vm.h"
#include "list.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "string.h"
#include "traps.h"

// FIXME: Remove this
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

// FIXME: Refactor this function
void vm_mmap(struct mm_struct *mm, unsigned long file, unsigned long addr,
             unsigned long len, unsigned long prot, unsigned long flags)
{
    struct vm_area_struct *vma = kmalloc(sizeof(struct vm_area_struct));
    vma->vm_start = addr;
    vma->vm_end = addr + len;
    vma->vm_flags = prot;
    vma->vm_file = file;
    list_add_tail(&vma->list, &mm->mmap);
}

struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr)
{
    struct list_head *pos;
    list_for_each(pos, &mm->mmap) {
        struct vm_area_struct *vma =
            list_entry(pos, struct vm_area_struct, list);
        if (vma->vm_start <= addr && addr < vma->vm_end)
            return vma;
    }
    return 0;
}

int access_error(unsigned long cause, struct vm_area_struct *vma)
{
    switch (cause) {
    case 12:
        return !(vma->vm_flags & VM_EXEC);
    case 13:
        return !(vma->vm_flags & (VM_READ | VM_WRITE));
    case 15:
        return !(vma->vm_flags & VM_WRITE);
    default:
        printk("access_error: unhandled cause %ld\n", cause);
    }
    return 0;
}

unsigned long *pte_alloc_map(struct mm_struct *mm, unsigned long addr)
{
    unsigned long *pte = (unsigned long *)mm->pgd;
    for (int level = 2; level >= 0; level--) {
        unsigned long idx = (addr >> (12 + 9 * level)) & 0x1ff;
        if (level == 0)
            return &pte[idx];
        if (pte[idx] == 0) {
            unsigned long *page_addr = (unsigned long *)kmalloc(PAGE_SIZE);
            memset(page_addr, 0, PAGE_SIZE);
            pte[idx] = virt_to_phys(page_addr) >> 2 | PAGE_PRESENT;
        }
        pte = (unsigned long *)phys_to_virt((pte[idx] & ~0x3ff) << 2);
    }
    return 0;
}

unsigned long mk_pte(unsigned long page_addr, unsigned long vm_flags)
{
    unsigned long page_prot =
        (PAGE_DIRTY | PAGE_ACCESSED | PAGE_USER | PAGE_PRESENT);
    if (vm_flags & VM_READ)
        page_prot |= PAGE_READ;
    if (vm_flags & VM_WRITE)
        page_prot |= PAGE_WRITE;
    if (vm_flags & VM_EXEC)
        page_prot |= PAGE_EXEC;
    return virt_to_phys(page_addr) >> 2 | page_prot;
}

void do_page_fault(struct pt_regs *regs)
{
    printk("page fault at %p (%ld)\n", regs->badaddr, regs->cause);
    struct mm_struct *mm = &get_current()->mm;
    struct vm_area_struct *vma = find_vma(mm, regs->badaddr);

    if (!vma || access_error(regs->cause, vma)) {
        printk("Segmentation fault\n");
        kthread_exit();
        return;
    }

    unsigned long addr = regs->badaddr & ~(PAGE_SIZE - 1);
    unsigned long *pte = pte_alloc_map(mm, addr);

    if (!*pte) {
        unsigned long *page_addr = kmalloc(PAGE_SIZE);
        if (vma->vm_file) {
            unsigned long off = addr - vma->vm_start;
            memcpy(page_addr, (unsigned long *)(vma->vm_file + off), PAGE_SIZE);
        } else {
            memset(page_addr, 0, PAGE_SIZE);
        }
        *pte = mk_pte((unsigned long)page_addr, vma->vm_flags);
    } else if (regs->cause == 15) { // do_wp_page

    } else {
        printk("do_page_fault: unhandled page fault (%ld)\n", regs->cause);
    }
}
