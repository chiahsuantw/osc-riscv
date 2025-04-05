#include "mm.h"
#include "printk.h"
#include "string.h"
#include "vm.h"

extern u8 _end;
static u8 *heap_top = &_end;

#define NUM_PAGES 0x140000

// TODO: Calculate the max cache index based on the page size
#define BUDDY_MAX_ORDER 10
#define CACHE_MAX_INDEX 6
#define CACHE_MIN_SIZE  32
#define CACHE_UNALLOC   -1

static struct page *mem_map;
static struct list_head free_area[BUDDY_MAX_ORDER + 1];
static struct list_head kmem_cache[CACHE_MAX_INDEX + 1];

static void *alloc_bootmem(unsigned long size)
{
    if (size < 0)
        return 0;
    void *p = heap_top;
    heap_top += size;
    return p;
}

void mem_init()
{
    // Set up the buddy system
    for (int i = 0; i <= BUDDY_MAX_ORDER; i++)
        INIT_LIST_HEAD(&free_area[i]);

    for (int i = 0; i <= CACHE_MAX_INDEX; i++)
        INIT_LIST_HEAD(&kmem_cache[i]);

    mem_map = alloc_bootmem(sizeof(struct page) * NUM_PAGES);
    for (int i = 0; i < NUM_PAGES; i++) {
        mem_map[i].order = 0;
        mem_map[i].used = 0;
        mem_map[i].cacheidx = CACHE_UNALLOC;
        mem_map[i].refcount = 0;
        INIT_LIST_HEAD(&mem_map[i].list);
        if (i % (1 << BUDDY_MAX_ORDER) == 0) {
            mem_map[i].order = BUDDY_MAX_ORDER;
            list_add_tail(&mem_map[i].list, &free_area[BUDDY_MAX_ORDER]);
        }
    }

    // Reserve the memory used by the kernel
    reserve_memory(0, (u64)virt_to_phys(heap_top));
}

static struct page *get_buddy(struct page *page, unsigned int order)
{
    unsigned int buddy_pfn = (unsigned int)(page - mem_map) ^ (1 << order);
    return &mem_map[buddy_pfn];
}

struct page *alloc_pages(unsigned int order)
{
    for (int i = order; i <= BUDDY_MAX_ORDER; i++) {
        if (list_empty(&free_area[i]))
            continue;

        struct page *page = list_first_entry(&free_area[i], struct page, list);
        list_del_init(&page->list);
        page->order = order;
        page->used = 1;
        page->refcount = 1;

        while (i > order) {
            i--;
            struct page *buddy = get_buddy(page, i);
            buddy->order = i;
            list_add(&buddy->list, &free_area[i]);
        }

        return page;
    }
    return 0;
}

void free_pages(struct page *page)
{
    struct page *current = page;
    unsigned order = page->order;

    while (order < BUDDY_MAX_ORDER) {
        struct page *buddy = get_buddy(current, order);
        if (buddy->used || buddy->order != order)
            break;
        list_del_init(&buddy->list);
        if (current > buddy)
            current = buddy;
        order++;
    }

    current->order = order;
    current->used = 0;
    current->refcount = 0;
    list_add(&current->list, &free_area[order]);
}

void *page_to_virt(struct page *page)
{
    return (void *)phys_to_virt((page - mem_map) * PAGE_SIZE);
}

struct page *virt_to_page(void *addr)
{
    return &mem_map[virt_to_phys(addr) / PAGE_SIZE];
}

void dump_buddy_info()
{
    // printk("Buddy info:\n");
    // for (int i = BUDDY_MAX_ORDER; i >= 0; i--) {
    //     int count = list_count_nodes(&free_area[i]);
    //     printk("free_area[%d]: %d", i, count);
    //     uart_hex(count);
    //     printk("\n");
    // }
}

void *kmem_cache_alloc(unsigned int index)
{
    if (list_empty(&kmem_cache[index])) {
        struct page *page = alloc_pages(0);
        page->cacheidx = index;
        unsigned long page_addr = phys_to_virt((page - mem_map) * PAGE_SIZE);
        unsigned int cache_size = CACHE_MIN_SIZE << index;
        for (int i = 0; i < PAGE_SIZE; i += cache_size) {
            struct object *obj = (struct object *)(page_addr + i);
            INIT_LIST_HEAD(&obj->list);
            list_add_tail(&obj->list, &kmem_cache[index]);
        }
    }
    struct object *obj =
        list_first_entry(&kmem_cache[index], struct object, list);
    list_del_init(&obj->list);
    return obj;
}

void kmem_cache_free(void *ptr)
{
    struct page *page = virt_to_page(ptr);
    struct object *obj = (struct object *)ptr;
    list_add_tail(&obj->list, &kmem_cache[page->cacheidx]);
}

void *kmalloc(unsigned int size)
{
    if (size <= 0)
        return 0;
    // TODO: Use (size > (PAGE_SIZE / 2)) instead
    if (size > (CACHE_MIN_SIZE << CACHE_MAX_INDEX)) {
        int order = 0;
        while ((PAGE_SIZE << order) < size)
            order++;
        struct page *page = alloc_pages(order);
        return page_to_virt(page);
    } else {
        int index = 0;
        while ((CACHE_MIN_SIZE << index) < size)
            index++;
        return kmem_cache_alloc(index);
    }
}

void kfree(void *ptr)
{
    struct page *page = virt_to_page(ptr);
    if (page->cacheidx == CACHE_UNALLOC) {
        if ((unsigned long)virt_to_phys(ptr) % PAGE_SIZE != 0)
            return;
        free_pages(page);
    } else {
        kmem_cache_free(ptr);
    }
}

void reserve_memory(u64 addr, u64 size)
{
    unsigned long start, end;
    start = addr & ~(PAGE_SIZE - 1);
    end = (addr + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (int order = BUDDY_MAX_ORDER; order >= 0; order--) {
        struct list_head *pos, *tmp;
        list_for_each_safe(pos, tmp, &free_area[order]) {
            struct page *curr = list_entry(pos, struct page, list);
            unsigned long page_start = (curr - mem_map) * PAGE_SIZE;
            unsigned long page_end = page_start + (PAGE_SIZE << order);
            if (page_start >= end || page_end <= start)
                continue;
            if (page_start >= start && page_end <= end) {
                list_del_init(&curr->list);
                curr->order = order;
                curr->used = 1;
            } else {
                struct page *half = get_buddy(curr, order - 1);
                list_del_init(&curr->list);
                curr->order = order - 1;
                half->order = order - 1;
                list_add(&curr->list, &free_area[order - 1]);
                list_add(&half->list, &free_area[order - 1]);
            }
        }
    }
}

unsigned long copy_user(void *to, const void *from, unsigned long n)
{
    // Set sstatus.SUM to 1
    asm("li t0, (1 << 18);"
        "csrs sstatus, t0;");
    memcpy(to, from, n);
    // Set sstatus.SUM to 0
    asm("li t0, (1 << 18);"
        "csrc sstatus, t0;");
    return 0;
}

void get_page(struct page *page)
{
    page->refcount++;
}

void put_page(struct page *page)
{
    page->refcount--;
    if (page->refcount == 0)
        free_pages(page);
}
