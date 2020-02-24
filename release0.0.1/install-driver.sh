sudo insmod kernel/megt2/megt2.ko
sudo mknod --mode=666 /dev/megt c `grep megt /proc/devices | awk '{print $1;}'` 0
