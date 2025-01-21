#include "vm.h"
#include "mm.h"
#include "string.h"
#include "traps.h"

static void pagewalk(unsigned long pgd, unsigned long va, unsigned long pa,
                     unsigned long prot)
{
    unsigned long *pte = (unsigned long *)pgd;
    for (int level = 2; level >= 0; level--) {
        unsigned long offset = (va >> (12 + 9 * level)) & 0x1ff;
        if (level == 0) {
            pte[offset] = pa | prot;
            return;
        }
        if ((pte[offset] & PAGE_PRESENT) == 0) {
            unsigned long page_addr = (unsigned long)kmalloc(PAGE_SIZE);
            memset(page_addr, 0, PAGE_SIZE);
            pte[offset] = virt_to_phys(page_addr) | PAGE_PRESENT;
        }
        pte = (unsigned long *)(phys_to_virt(pte[offset] & ~0xfff));
    }
}

void map_pages(unsigned long pgd, unsigned long va, unsigned long size,
               unsigned long pa, unsigned long prot)
{
    for (int i = 0; i < size; i += PAGE_SIZE)
        pagewalk(pgd, va + i, pa + i, prot);
}
