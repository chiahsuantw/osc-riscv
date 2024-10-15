#include "devtree.h"
#include "list.h"
#include "printk.h"
#include "sbi.h"
#include "shell.h"
#include "uart.h"

void switch_to_user_mode()
{
    asm("csrw sepc, ra;");
    asm("sret");
}

void trap_handler()
{
    printk("Kernel Trap!\n");
    while (1)
        ;
}

void enable_interrupt()
{
    // Set sstatus.SIE to 1
    asm("csrsi sstatus, (1 << 1)");
}

void enable_timer_interrupt()
{
    // Set sie.TSIE to 1
    asm("li t0, (1 << 5);"
        "csrs sie, t0;");
}

int start_kernel()
{
    printk("\nNYCU OSC RISC-V KERNEL\n");

    struct my_struct {
        int data;
        struct list_head list;
    };

    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);

    struct my_struct item1, item2;
    item1.data = 1;
    item2.data = 2;
    INIT_LIST_HEAD(&item1.list);
    INIT_LIST_HEAD(&item2.list);
    list_add(&item1.list, &my_list);
    list_add(&item2.list, &my_list);

    struct list_head *current;
    list_for_each(current, &my_list) {
        printk("List Item: ");
        struct my_struct *s = list_entry(current, struct my_struct, list);
        uart_hex(s->data);
        printk("\n");
    }

    run_shell();
    return 0;
}
