
obj-m := megt2.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
VERBOSE = 0

default:
        $(MAKE) -C $(KDIR) M=$(PWD) KBUILD_VERBOSE=$(VERBOSE) modules

clean:
        rm -f *.o *.ko *.mod.c
