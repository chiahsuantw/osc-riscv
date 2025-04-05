#pragma once

#include "list.h"
#include "types.h"

#define PAGE_SIZE 4096

struct page {
    unsigned int order;
    unsigned int used;
    unsigned int cacheidx;
    unsigned int refcount;
    struct list_head list;
};

struct object {
    struct list_head list;
};

struct page *alloc_pages(unsigned int order);
void free_pages(struct page *page);
void *page_to_virt(struct page *page);
struct page *virt_to_page(void *addr);

void *kmalloc(unsigned int size);
void kfree(void *ptr);

void mem_init();
void reserve_memory(u64 addr, u64 size);
void dump_buddy_info();

unsigned long copy_user(void *to, const void *from, unsigned long n);

void get_page(struct page *page);
void put_page(struct page *page);
