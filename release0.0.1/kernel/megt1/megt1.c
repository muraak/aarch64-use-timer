#include <linux/module.h>

static void enable_timer(void);
static void disable_timer(void);

static void enable_timer(void)
{
    volatile int tmp = 0;
    volatile int tmp2 = 0;
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    printk("[MEGT]current cntkctl_el1: %08x", tmp);
    tmp |= (3 << 0);
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("msr cntkctl_el1, %0":: "r" (tmp));
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp2));
    printk("[MEGT]modified cntkctl_el1: %08x\n", tmp2);
    return;
}

static void disable_timer(void)
{
    volatile int tmp = 0;
    volatile int tmp2 = 0;
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    printk("[MEGT]current cntkctl_el1: %08x", tmp);
    tmp &= 0xfffffffe;
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("msr cntkctl_el1, %0":: "r" (tmp));
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp2));
    printk("[MEGT]modified cntkctl_el1: %08x\n", tmp2);
    return;
}

static int __init megt_init(void)
{
    printk("[MEGT]init.\n");
    enable_timer();
    return 0;
}

static void __exit megt_exit(void)
{
    printk("[MEGT]exit.\n");
    disable_timer();
    return;
}

module_init(megt_init);
module_exit(megt_exit);
MODULE_LICENSE("GPL");

