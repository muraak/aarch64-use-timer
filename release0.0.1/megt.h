#ifndef _MEGT_H_
#define _MEGT_H_

#include <linux/ioctl.h>

#define MEGT_IOC_MAGIC 'k'
#define MEGT_IOC_MAXNR 3

#define MEGT_IOC_ENATIMER _IOR(MEGT_IOC_MAGIC,  1, int)
#define MEGT_IOC_DISTIMER _IOR(MEGT_IOC_MAGIC,  2, int)
#define MEGT_IOC_CHKTIMER _IOR(MEGT_IOC_MAGIC,  3, int)

#endif // _MEGT_H_
