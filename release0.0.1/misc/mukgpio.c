#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "mukgpio.h"

#define MUKGPIO_MINOR_BASE 0
#define MUKGPIO_MINOR_NUM  1

static struct cdev mukgpio_cdev;
static struct class *mukgpio_class = NULL; 

// https://github.com/jwatte/jetson-gpio-example/blob/master/gpiomem.cpp

#define GPIO_1     0x6000d000
#define GPIO_2     0x6000d100
#define GPIO_3     0x6000d200
#define GPIO_4     0x6000d300
#define GPIO_5     0x6000d400
#define GPIO_6     0x6000d500
#define GPIO_7     0x6000d600
#define GPIO_8     0x6000d700

typedef struct MukGpioRegs {
    volatile uint32_t CNF[4];     // +00h
    volatile uint32_t OE[4];      // +10h
    volatile uint32_t OUT[4];     // +20h
    volatile uint32_t IN[4];      // +30h
    volatile uint32_t INT_STA[4]; // +40h
    volatile uint32_t INT_ENB[4]; // +50h
    volatile uint32_t INT_LVL[4]; // +60h
    volatile uint32_t INT_CLR[4]; // +70h
}MukGpioRegs;

static int mukgpio_major   = 0;
// static int mukgpio_minor   = MUKGPIO_MINOR_BASE;
// static int mukgpio_nr_devs = MUKGPIO_MINOR_NUM;	/* number of bare mukgpio devices */

static int mukgpio_open(struct inode *inode, struct file *file)
{
	printk("mukgpio_open\n");
	return 0;
}

static int mukgpio_close(struct inode *inode, struct file *file)
{
	printk("mukgpio_close\n");
	return 0;
}

static ssize_t mukgpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mukgpio_read\n");
	return count;
}

static ssize_t mukgpio_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mukgpio_write\n");
	return count;
}

static long mukgpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
	int retval = 0;
    
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != MUKGPIO_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > MUKGPIO_IOC_MAXNR) return -ENOTTY;

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
	    case MUKGPIO_IOCSETDIROUT:
            // TO BE IMPLEMENTED!!
		    break;
	    case MUKGPIO_IOCSETDIRIN:
            // TO BE IMPLEMENTED!!
		    break;
        case MUKGPIO_IOCSETOUTON:
            // TO BE IMPLEMENTED!!
            {
                // In this operation, arg indicates the index of gpio pin
                // (e.g. gpio_**, where ** is 0 ~ 255)
                int port, offset = 0;
                volatile uint32_t tmp = 0;
                MukGpioRegs * regs;
                if(arg > 255) return -EFAULT;
                port = arg >> 5; // Each port manages 32 gpio pins.
                offset = arg & 31; // This is equivalent to `arg % 32`.
                regs = (MukGpioRegs *)ioremap_nocache(GPIO_1 + (0x100 * port), sizeof(MukGpioRegs));
                tmp = regs->OUT[0];
                tmp = tmp | (1 << offset);
                regs->OUT[0] = tmp;
                while(regs->OUT[0] != tmp){}
                iounmap((void*)regs);
            }
		    break;
        case MUKGPIO_IOCSETOUTOFF:
            // TO BE IMPLEMENTED!!
		    break;
        default:  /* redundant, as cmd was checked against MAXNR */
		    return -ENOTTY;
    }

    return retval;

}

struct file_operations mukgpio_fops = {
	.owner =    THIS_MODULE,
	.llseek =   NULL,
	.read =     mukgpio_read,
	.write =    mukgpio_write,
    .unlocked_ioctl = mukgpio_ioctl,
	.compat_ioctl   = mukgpio_ioctl, // for 32-bit App
	.open =     mukgpio_open,
	.release =  mukgpio_close,
};

static int __init mukgpio_init_module(void)
{
    int result;
	dev_t dev = 0;

    // Get an available major number
	result = alloc_chrdev_region(&dev, MUKGPIO_MINOR_BASE, MUKGPIO_MINOR_NUM, "mukgpio");
	mukgpio_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "mukgpio: can't get major %d\n", mukgpio_major);
		return result;
	}

    // dev = MKDEV(mukgpio_major, MUKGPIO_MINOR_BASE);

    // Initialize device
    cdev_init(&mukgpio_cdev, &mukgpio_fops);
    mukgpio_cdev.owner = THIS_MODULE;

    // Register this device to the kernel
    result = cdev_add(&mukgpio_cdev, dev, MUKGPIO_MINOR_NUM);
    if (result != 0) {
        printk(KERN_ERR  "mukgpio: can't add device(%d)\n", result);
        unregister_chrdev_region(dev, MUKGPIO_MINOR_NUM);
        return -1;
    }

    // Register to the class(e.g. Make `/sys/class/mukgpio/`)
    mukgpio_class = class_create(THIS_MODULE, "mukgpio");
    if (IS_ERR(mukgpio_class)) {
        printk(KERN_ERR  "class_create\n");
        cdev_del(&mukgpio_cdev);
        unregister_chrdev_region(dev, MUKGPIO_MINOR_NUM);
        return -1;
    }

    return 0;
}

static void  __exit mukgpio_cleanup_module(void)
{
    dev_t dev = MKDEV(mukgpio_major, MUKGPIO_MINOR_BASE);
    class_destroy(mukgpio_class);
    cdev_del(&mukgpio_cdev);
    unregister_chrdev_region(dev, MUKGPIO_MINOR_NUM);
}

module_init(mukgpio_init_module);
module_exit(mukgpio_cleanup_module);
MODULE_LICENSE("GPL");
