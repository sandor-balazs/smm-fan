# Utility to enable / disable SMM BIOS fan control on Dell laptops (64-bit)

[![Build Status][1]][2]

Download: [smm-fan-dist.tar.bz2][3]

On some Dell laptops SMM BIOS fan speed control prevents proper setting of fan
speed from userspace because it takes back control [one or two seconds after disabling it][4].

There is a utility to change this behavior in [i8kutils][5] however it is
only a 32-bit version. Trying to compile it on a 64-bit system without
multilib gives the following error message:
```
$ gcc -m32 -o smm smm.c
In file included from /usr/include/features.h:392:0,
                 from /usr/include/stdio.h:27,
                 from smm.c:27:
/usr/include/gnu/stubs.h:7:27: fatal error: gnu/stubs-32.h: No such file or directory
 # include <gnu/stubs-32.h>
                           ^
compilation terminated.
```
Most sites suggest that on 64-bit systems the only solution is to install
gcc-multilib \[[1][6],[2][7],[3][8],[4][9]\]. As it turned out the original
`smm.c` code can be converted to a 64-bit version (saving me from having to
change Gentoo profile from no-multilib to multilib).

The current 64-bit solution is based on:
* `dell-smm-hwmon.c` -- Linux driver for accessing the SMM BIOS on Dell laptops
* `smm.c`            -- Utility to test SMM BIOS calls on Inspiron 8000 laptops from i8kutils

Tested on Dell Latitude E6530 and works flawlessly. But as usual, use at your own risk.

Usage:
```
$ make
# ./smm-fan
./smm-fan: expected 1 parameter defining state

usage: ./smm-fan state
        set SMM BIOS fan control state = { 0, 1 }; (0: disabled, 1: enabled)
```
Disabling SMM BIOS fan control:
```
# ./smm-fan 0
```

[1]: https://travis-ci.org/sandor-balazs/smm-fan.svg?branch=master
[2]: https://travis-ci.org/sandor-balazs/smm-fan
[3]: https://github.com/sandor-balazs/smm-fan/releases/download/v1.0.0/smm-fan-dist.tar.bz2
[4]: https://bugs.launchpad.net/i8kutils/+bug/410596
[5]: https://launchpad.net/i8kutils
[6]: https://wiki.archlinux.org/index.php/fan_speed_control#Disable_BIOS_fan_speed_control
[7]: https://askubuntu.com/questions/63588/how-do-i-get-fan-control-working/398635#398635
[8]: https://superuser.com/questions/533102/laptop-fan-is-always-on-using-linux-mint-14/695796#695796
[9]: https://unix.stackexchange.com/questions/63220/ubuntu-12-10-fan-overspeeding/107453#107453

