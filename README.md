# Enable to use generic timer on AArch64 Linux

This project includes the kernel module `megt2` that enables to
use generic timer in user-land on AArch64 Linux.

Please check `release0.0.1` in detail.

### Make kernel module

```
cd release0.0.1/kernel/megt2
make clean
make
```

### Install kernel module

```
cd release0.0.1
sh install-driver.sh
```

### Enable/Check/Disable generic timer for userland

```
# build user commands
cd release0.0.1
sh build.sh

# enable timer
./ena-timer.out

# check whether timer is enabled or disabled
./chk-timer.out

# probe timer works fine 
./probe-timer.out

# disable timer
./dis-timer.out
```

### Uninstall kernel module

```
cd release0.0.1
sh uninstall-driver.sh
```


Happy develop!
Muraak.
