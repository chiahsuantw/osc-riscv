#include "mm.h"
#include "types.h"

extern u8 __bss_end;

static u8 *heap_top = &__bss_end;

void *alloc(unsigned long size)
{
    if (size < 0)
        return 0;
    void *p = heap_top;
    heap_top += size;
    return p;
}
