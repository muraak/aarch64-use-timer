#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include"megt.h"

int main(void)
{
    int fd = 0;
    int status = 0;
    
	if ((fd = open("/dev/megt", O_RDWR)) < 0) perror("open");
    status = ioctl(fd, MEGT_IOC_CHKTIMER, NULL);
    if (close(fd) != 0) perror("close");

    if(status < 0) {
        if(errno == EFAULT) {
            printf("Timer is disabled now.¥n");
        }
        else {
            printf("IOCTL is failed.¥n");
        }
    }
    else {
        printf("Timer is enabled now.¥n");
    }

    return 0;
}
