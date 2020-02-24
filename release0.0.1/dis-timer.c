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
    
	if ((fd = open("/dev/megt", O_RDWR)) < 0) perror("open");
    if (ioctl(fd, MEGT_IOC_DISTIMER, NULL) < 0) perror("ioctl_set");
    if (close(fd) != 0) perror("close");

    printf("Timer was enabled witout error!\n");
    printf("But you sould check with `chk-timer` command.\n");
    return 0;
}
