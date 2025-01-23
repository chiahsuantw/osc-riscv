#pragma once

#include "list.h"
#include "traps.h"

#define PAGE_OFFSET 0xffffffc000000000

#define PAGE_PRESENT  (1 << 0)
#define PAGE_READ     (1 << 1)
#define PAGE_WRITE    (1 << 2)
#define PAGE_EXEC     (1 << 3)
#define PAGE_USER     (1 << 4)
#define PAGE_GLOBAL   (1 << 5)
#define PAGE_ACCESSED (1 << 6)
#define PAGE_DIRTY    (1 << 7)
#define PAGE_SOFT     (3 << 8)

#define PAGE_BASE (PAGE_DIRTY | PAGE_ACCESSED | PAGE_USER | PAGE_PRESENT)
#define PAGE_RX   (PAGE_BASE | PAGE_READ | PAGE_EXEC)
#define PAGE_RW   (PAGE_BASE | PAGE_READ | PAGE_WRITE)

struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    unsigned long vm_page_prot;
    struct list_head list;
};

#define virt_to_phys(x) ((unsigned long)(x) - PAGE_OFFSET)
#define phys_to_virt(x) ((unsigned long)(x) + PAGE_OFFSET)

void map_pages(unsigned long pgd, unsigned long va, unsigned long size,
               unsigned long pa, unsigned long prot);
void do_page_fault(struct pt_regs *regs);
