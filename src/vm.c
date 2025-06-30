#include "vm.h"
#include "list.h"
#include "mm.h"
#include "printk.h"
#include "sched.h"
#include "string.h"
#include "traps.h"

#define HPAGE_SIZE (1UL << 30)
#define HPAGE_NR   (HPAGE_SIZE / PAGE_SIZE)

unsigned long
    __attribute__((section(".data"), aligned(PAGE_SIZE))) pg_dir[512] = { 0 };

void setup_vm()
{
    for (int i = 0; i < NUM_PAGES / HPAGE_NR; i++)
        pg_dir[256 + i] = (i * HPAGE_NR) << 10 | PAGE_PRESENT | PAGE_READ |
                          PAGE_WRITE | PAGE_EXEC | PAGE_DIRTY | PAGE_ACCESSED;
    asm("sfence.vma");
    asm("csrw satp, %0" ::"r"(0x8UL << 60 | (unsigned long)pg_dir >> 12));
}

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
    vma->vm_mm = mm;
    vma->vm_flags = prot;
    vma->vm_file = file;
    list_add_tail(&vma->list, &mm->mmap);
}

void *pte_to_virt(unsigned long pte)
{
    return (void *)phys_to_virt((pte & ~0x3ff) << 2);
}

void copy_page_range(struct vm_area_struct *dst_vma,
                     struct vm_area_struct *src_vma)
{
    for (unsigned long addr = src_vma->vm_start; addr < src_vma->vm_end;
         addr += PAGE_SIZE) {
        unsigned long *src_pt = (unsigned long *)src_vma->vm_mm->pgd;
        unsigned long *dst_pt = (unsigned long *)dst_vma->vm_mm->pgd;
        for (int level = 2; level >= 0; level--) {
            long idx = (addr >> (12 + 9 * level)) & 0x1ff;
            if (src_pt[idx] != 0 && dst_pt[idx] == 0) {
                unsigned long *page_addr = (unsigned long *)kmalloc(PAGE_SIZE);
                memset(page_addr, 0, PAGE_SIZE);
                dst_pt[idx] = virt_to_phys(page_addr) >> 2 | PAGE_PRESENT;
                if (level == 0) {
                    src_pt[idx] &= ~PAGE_WRITE;
                    dst_pt[idx] = src_pt[idx];
                    get_page(virt_to_page(pte_to_virt(src_pt[idx])));
                }
            }
            src_pt = (unsigned long *)phys_to_virt((src_pt[idx] & ~0x3ff) << 2);
            dst_pt = (unsigned long *)phys_to_virt((dst_pt[idx] & ~0x3ff) << 2);
        }
    }
}

void dup_mmap(struct mm_struct *mm, struct mm_struct *oldmm)
{
    struct list_head *pos;
    list_for_each(pos, &oldmm->mmap) {
        struct vm_area_struct *old_vma =
            list_entry(pos, struct vm_area_struct, list);
        struct vm_area_struct *vma = kmalloc(sizeof(struct vm_area_struct));
        memcpy(vma, old_vma, sizeof(struct vm_area_struct));
        vma->vm_mm = mm;
        copy_page_range(vma, old_vma);
        list_add_tail(&vma->list, &mm->mmap);
    }
    asm("sfence.vma");
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
    case EXC_INST_PAGE_FAULT:
        return !(vma->vm_flags & VM_EXEC);
    case EXC_LOAD_PAGE_FAULT:
        return !(vma->vm_flags & (VM_READ | VM_WRITE));
    case EXC_STORE_PAGE_FAULT:
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

    if (!*pte) { /* Demand paging */
        unsigned long *page_addr = kmalloc(PAGE_SIZE);
        if (vma->vm_file) {
            unsigned long off = addr - vma->vm_start;
            memcpy(page_addr, (unsigned long *)(vma->vm_file + off), PAGE_SIZE);
        } else {
            memset(page_addr, 0, PAGE_SIZE);
        }
        *pte = mk_pte((unsigned long)page_addr, vma->vm_flags);
    } else if (regs->cause == 15 && !(*pte & PAGE_WRITE)) { /* Copy-on-Write */
        struct page *old_page = virt_to_page(pte_to_virt(*pte));
        if (old_page->refcount > 1) {
            struct page *new_page = alloc_pages(0);
            memcpy(page_to_virt(new_page), page_to_virt(old_page), PAGE_SIZE);
            *pte = mk_pte((unsigned long)page_to_virt(new_page), vma->vm_flags);
            put_page(old_page);
        } else {
            *pte |= PAGE_WRITE;
        }
    } else {
        printk("do_page_fault: unhandled page fault (%ld)\n", regs->cause);
        while (1)
            ;
    }

    asm("sfence.vma");
}
