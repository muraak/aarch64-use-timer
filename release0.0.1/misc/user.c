#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "mukgpio.h"

int main()
{
    int fd = 0;
    
	if ((fd = open("/dev/mukgpio", O_RDWR)) < 0) perror("open");

	if (ioctl(fd, MUKGPIO_IOCSETOUTON, 38) < 0) perror("ioctl_set");

	if (close(fd) != 0) perror("close");

	return 0;
}
