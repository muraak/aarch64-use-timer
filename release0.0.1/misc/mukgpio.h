#ifndef _MUKGPIO_H_
#define _MUKGPIO_H_

#include <linux/ioctl.h>

/////////////////////
// Ioctl definitions
/////////////////////

#define MUKGPIO_IOC_MAGIC  'm'

#define MUKGPIO_IOCSETDIROUT _IOR(MUKGPIO_IOC_MAGIC,  1, int)
#define MUKGPIO_IOCSETDIRIN  _IOR(MUKGPIO_IOC_MAGIC,  2, int)
#define MUKGPIO_IOCSETOUTON  _IOR(MUKGPIO_IOC_MAGIC,  3, int)
#define MUKGPIO_IOCSETOUTOFF _IOR(MUKGPIO_IOC_MAGIC,  4, int)

#define MUKGPIO_IOC_MAXNR 4

#endif // _MUKGPIO_H_
