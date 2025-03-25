#pragma once

#include "list.h"
#include "traps.h"

#define PAGE_OFFSET 0xffffffc000000000

/* Page protection bits */
#define PAGE_PRESENT  (1 << 0)
#define PAGE_READ     (1 << 1)
#define PAGE_WRITE    (1 << 2)
#define PAGE_EXEC     (1 << 3)
#define PAGE_USER     (1 << 4)
#define PAGE_GLOBAL   (1 << 5)
#define PAGE_ACCESSED (1 << 6)
#define PAGE_DIRTY    (1 << 7)
#define PAGE_SOFT     (3 << 8)

// #define PAGE_BASE (PAGE_DIRTY | PAGE_ACCESSED | PAGE_USER | PAGE_PRESENT)

/* Flags (vm_flags) for virtual memory areas */
#define VM_NONE  0x0
#define VM_READ  0x1
#define VM_WRITE 0x2
#define VM_EXEC  0x4

/* Protections and flags for mmap */
#define PROT_NONE     0x0
#define PROT_READ     0x1
#define PROT_WRITE    0x2
#define PROT_EXEC     0x4
#define MAP_ANONYMOUS 0x20
#define MAP_POPULATE  0x8000

struct mm_struct {
    struct list_head mmap;
    unsigned long *pgd;
};

struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    struct mm_struct *vm_mm;
    unsigned long vm_flags;
    unsigned long vm_file;
    struct list_head list;
};

#define virt_to_phys(x) ((unsigned long)(x) - PAGE_OFFSET)
#define phys_to_virt(x) ((unsigned long)(x) + PAGE_OFFSET)

// TODO: Refactor this
#ifdef __QEMU__
#define PGD_BASE 0x80100000
#else
#define PGD_BASE 0x40100000
#endif

void map_pages(struct mm_struct *mm, unsigned long va, unsigned long size,
               unsigned long pa, unsigned long prot);
void vm_mmap(struct mm_struct *mm, unsigned long file, unsigned long addr,
             unsigned long len, unsigned long prot, unsigned long flags);
void dup_mmap(struct mm_struct *mm, struct mm_struct *oldmm);
void do_page_fault(struct pt_regs *regs);
