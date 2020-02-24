#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "../../megt.h"

#define MEGT_MINOR_BASE 0
#define MEGT_MINOR_NUM  1

static struct cdev megt_cdev;
static struct class *megt_class = NULL;

static int megt_major   = 0;

static void enable_timer(void);
static void disable_timer(void);

static void enable_timer(void)
{
    volatile int tmp = 0;
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    tmp |= (3 << 0);
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("msr cntkctl_el1, %0":: "r" (tmp));
    return;
}

static void disable_timer(void)
{
    volatile int tmp = 0;
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    tmp &= 0xfffffffe;
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    asm volatile("msr cntkctl_el1, %0":: "r" (tmp));
    return;
}

static int is_enabled(void)
{
    volatile int tmp = 0;
    asm volatile ("" : : : "memory");
    asm volatile("mrs %0, cntkctl_el1" : "=r" (tmp));
    asm volatile("":::"memory");
    dsb(sy);
    dmb(sy);
    asm volatile("isb sy");
    return ((tmp & 0x00000001) == 0x00000001);
}

static int megt_open(struct inode *inode, struct file *file)
{
	printk("[MEGT]open\n");
	return 0;
}

static int megt_close(struct inode *inode, struct file *file)
{
	printk("[MEGT]close\n");
	return 0;
}

static ssize_t megt_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("[MEGT]read\n");
	return count;
}

static ssize_t megt_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("[MEGT]write\n");
	return count;
}

static long megt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
	int retval = 0;
    
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != MEGT_IOC_MAGIC) {
        printk("[MEGT]ioctl: magic not matched:%d\n", _IOC_TYPE(cmd));
        return -ENOTTY;
    }
	if (_IOC_NR(cmd) > MEGT_IOC_MAXNR)
    {
        printk("[MEGT]ioctl: aboved maxnr:%d\n", _IOC_NR(cmd));
        return -ENOTTY;
    }
	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

    switch(cmd) {
	    case MEGT_IOC_ENATIMER:
            enable_timer();
            printk("[MEGT]enable_timer\n");
		    break;
	    case MEGT_IOC_DISTIMER:
            disable_timer();
            printk("[MEGT]disable_timer\n");
		    break;
        case MEGT_IOC_CHKTIMER:
            if(is_enabled()) {
                return retval;
            }
            else {
                return -EFAULT;
            }
		    break;
        default:  /* redundant, as cmd was checked against MAXNR */
		    return -ENOTTY;
    }

    return retval;
}

struct file_operations megt_fops = {
	.owner =    THIS_MODULE,
	.llseek =   NULL,
	.read =     megt_read,
	.write =    megt_write,
    .unlocked_ioctl = megt_ioctl,
	.compat_ioctl   = megt_ioctl, // for 32-bit App
	.open =     megt_open,
	.release =  megt_close,
};

static int __init megt_init_module(void)
{
    int result;
	dev_t dev = 0;

    // Get an available major number
	result = alloc_chrdev_region(&dev, MEGT_MINOR_BASE, MEGT_MINOR_NUM, "megt");
	megt_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "[MEGT]can't get major %d\n", megt_major);
		return result;
	}

    // Initialize device
    cdev_init(&megt_cdev, &megt_fops);
    megt_cdev.owner = THIS_MODULE;

    // Register this device to the kernel
    result = cdev_add(&megt_cdev, dev, MEGT_MINOR_NUM);
    if (result != 0) {
        printk(KERN_ERR  "[MEGT]can't add device(%d)\n", result);
        unregister_chrdev_region(dev, MEGT_MINOR_NUM);
        return -1;
    }

    // Register to the class(e.g. Make `/sys/class/megt/`)
    megt_class = class_create(THIS_MODULE, "megt");
    if (IS_ERR(megt_class)) {
        printk(KERN_ERR  "[MEGT]class_create failed\n");
        cdev_del(&megt_cdev);
        unregister_chrdev_region(dev, MEGT_MINOR_NUM);
        return -1;
    }

    return 0;
}

static void  __exit megt_cleanup_module(void)
{
    dev_t dev = MKDEV(megt_major, MEGT_MINOR_BASE);
    class_destroy(megt_class);
    cdev_del(&megt_cdev);
    unregister_chrdev_region(dev, MEGT_MINOR_NUM);
}

module_init(megt_init_module);
module_exit(megt_cleanup_module);
MODULE_LICENSE("GPL");
