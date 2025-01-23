#include "vm.h"
#include "mm.h"
#include "printk.h"
#include "string.h"
#include "traps.h"

// FIXME: Load access fault
static void pagewalk(unsigned long pgd, unsigned long va, unsigned long pa,
                     unsigned long prot)
{
    unsigned long *pte = (unsigned long *)pgd;
    for (int level = 2; level >= 0; level--) {
        unsigned long idx = (va >> (12 + 9 * level)) & 0x1ff;
        if (level == 0) {
            pte[idx] = pa | prot;
            return;
        }
        if (pte[idx] == 0) {
            unsigned long *page_addr = (unsigned long *)kmalloc(PAGE_SIZE);
            memset(page_addr, 0, PAGE_SIZE);
            pte[idx] = virt_to_phys(page_addr) | PAGE_PRESENT;
        }
        pte = (unsigned long *)phys_to_virt(pte[idx]);
    }
}

void map_pages(unsigned long pgd, unsigned long va, unsigned long size,
               unsigned long pa, unsigned long prot)
{
    for (int i = 0; i < size; i += PAGE_SIZE)
        pagewalk(pgd, va + i, pa + i, prot);
}

void do_page_fault(struct pt_regs *regs)
{
    printk("[PANIC] Page fault\n");
    printk("sepc: %p\n", regs->epc);
    printk("sstatus: %p\n", regs->status);
    printk("scause: %p\n", regs->cause);
    printk("stval: %p\n", regs->badaddr);
    while (1)
        ;
}
