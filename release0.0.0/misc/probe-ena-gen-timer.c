#include <linux/module.h>

#define GET_CNTFRQ(cntfrq) asm volatile ("" : : : "memory");\
						   asm volatile("mrs %0, cntfrq_el0" : "=r" (cntfrq));

#define GETTIME(count) asm volatile ("" : : : "memory");\
                	   asm volatile("mrs %0, cntpct_el0" : "=r" (count));

static void enable_timer()
{
    volatile int tmp = 0; 
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    printk("[MEGT]current cntkctl_el1: %08x", tmp);
    tmp |= (1 << 9);
    asm volatile("msr pmintenset_el1, %0":: "r"tmp);
    printk("[MEGT]modified cntkctl_el1: %08x", tmp);

    return;
}

static int megt_init(void)
{
    printk("[MEGT]init.\n");
    enable_timer();
    return 0;
}

static void megt_exit(void)
{
    printk("[MEGT]exit.\n");
}

module_init(megt_init);
module_exit(megt_exit);
