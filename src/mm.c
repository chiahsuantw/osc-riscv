#include "mm.h"
#include "printk.h"

extern u8 _end;
static u8 *heap_top = &_end;

#define BUDDY_MAX_ORDER 10
#define CACHE_MAX_ORDER 6

#define NUM_PAGES 0x40000

static struct page *mem_map;
static struct list_head free_area[BUDDY_MAX_ORDER + 1];
static struct list_head kmem_cache[CACHE_MAX_ORDER + 1];

static void *alloc(unsigned long size)
{
    if (size < 0)
        return 0;
    void *p = heap_top;
    heap_top += size;
    return p;
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
        list_del_entry(&page->list);
        page->order = order;
        page->used = 1;

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
        list_del_entry(&buddy->list);
        if (current > buddy) {
            struct page *tmp = current;
            current = buddy;
            buddy = tmp;
        }
        order++;
    }

    current->order = order;
    current->used = 0;
    list_add(&current->list, &free_area[order]);
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
    return 0;
}

void kmem_cache_free(void *ptr)
{
}

void *kmalloc(unsigned int size)
{
    return 0;
}

void kfree(void *ptr)
{
}

void mem_init()
{
    // Set up the buddy system
    for (int i = 0; i <= BUDDY_MAX_ORDER; i++)
        INIT_LIST_HEAD(&free_area[i]);

    for (int i = 0; i <= CACHE_MAX_ORDER; i++)
        INIT_LIST_HEAD(&kmem_cache[i]);

    mem_map = alloc(sizeof(struct page) * NUM_PAGES);
    for (int i = 0; i < NUM_PAGES; i++) {
        mem_map[i].order = 0;
        mem_map[i].used = 0;
        mem_map[i].cacheidx = -1;
        INIT_LIST_HEAD(&mem_map[i].list);
        if (i % (1 << BUDDY_MAX_ORDER) == 0) {
            mem_map[i].order = BUDDY_MAX_ORDER;
            list_add_tail(&mem_map[i].list, &free_area[BUDDY_MAX_ORDER]);
        }
    }

    // TODO: Reserve the memory used by the kernel
}
