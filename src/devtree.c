#include "devtree.h"
#include "printk.h"
#include "string.h"
#include "uart.h"
#include "utils.h"

void *DTB_BASE = (void *)0x87000000;

void fdt_traverse(const char *target, void (*callback)(void *))
{
    // printk("[INFO] Dtb is loaded at %p\n", DTB_BASE);

    struct fdt_header *hdr = (struct fdt_header *)DTB_BASE;
    if (be2le(hdr->magic) != 0xd00dfeed) {
        printk("[ERROR] FDT header magic does not match!\n");
        return;
    }

    u8 *dt_struct = (u8 *)DTB_BASE + be2le(hdr->off_dt_struct);
    u8 *dt_strings = (u8 *)DTB_BASE + be2le(hdr->off_dt_strings);
    u32 struct_size = be2le(hdr->size_dt_struct);

    u8 *p = dt_struct;
    while (p < dt_struct + struct_size) {
        u32 token = be2le(*(u32 *)p);
        p += 4;

        switch (token) {
        case FDT_BEGIN_NODE:
            printk("[INFO] Node: %s\n", p);
            p += align(strlen((char *)p) + 1, 4);
            break;
        case FDT_END_NODE:
            break;
        case FDT_PROP: {
            u32 len = be2le(*(u32 *)p);
            u32 nameoff = be2le(*(u32 *)(p + 4));
            if (!strcmp((char *)(dt_strings + nameoff), target)) {
                // callback((void *)(be2le(*(u32 *)(p + 8))));
                return;
            }
            p += 8 + align(len, 4);
            break;
        }
        case FDT_NOP:
            break;
        case FDT_END:
            break;
        }
    }
}
