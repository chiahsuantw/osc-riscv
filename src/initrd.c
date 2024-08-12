#include "initrd.h"
#include "printk.h"
#include "string.h"
#include "utils.h"

void *INITRD_BASE = (void *)0x84200000;

void *initrd_get_next_hdr(const void *ptr)
{
    if (!memcmp(ptr + sizeof(struct cpio_t), "TRAILER!!!", 10))
        return 0;
    struct cpio_t *hdr = (struct cpio_t *)ptr;
    int namesize = hextoi(hdr->namesize, 8);
    int filesize = hextoi(hdr->filesize, 8);
    int headsize = align(sizeof(struct cpio_t) + namesize, 4);
    int datasize = align(filesize, 4);
    return (void *)(ptr + headsize + datasize);
}

void initrd_list()
{
    const char *ptr = INITRD_BASE;
    while (initrd_get_next_hdr(ptr)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;
        int namesize = hextoi(hdr->namesize, 8);
        char pathname[namesize];
        strncpy(pathname, ptr + sizeof(struct cpio_t), namesize);
        printk("%s\n", pathname);
        ptr = initrd_get_next_hdr(ptr);
    }
}

void initrd_cat(const char *target)
{
    const char *ptr = INITRD_BASE;
    while (initrd_get_next_hdr(ptr)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;
        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);
        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        if (!memcmp(ptr + sizeof(struct cpio_t), target, namesize)) {
            char data[filesize + 1];
            memset(data, 0, filesize + 1);
            strncpy(data, ptr + headsize, filesize);
            printk("%s\n", data);
            return;
        }
        ptr = initrd_get_next_hdr(ptr);
    }
    printk("cat: %s not found\n", target);
}
