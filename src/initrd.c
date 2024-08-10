#include "initrd.h"
#include "string.h"
#include "uart.h"
#include "utils.h"

void *INITRD_BASE = (void *)0x84200000;

void initrd_traverse(int (*callback)(const void *))
{
    const char *ptr = INITRD_BASE;

    while (memcmp(ptr + sizeof(struct cpio_t), "TRAILER!!!", 10)) {
        struct cpio_t *hdr = (struct cpio_t *)ptr;

        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);

        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        int datasize = align(filesize, 4);

        char pathname[namesize];
        strncpy(pathname, ptr + sizeof(struct cpio_t), namesize);
        uart_puts(pathname);
        uart_puts("\n");

        ptr += headsize + datasize;
    }
}
