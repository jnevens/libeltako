About
=====

Library plus tools to communicate with the Eltako Series 14 domotics system.

Installation
============

On UNIX-like systems (including Linux, Mac OS X, MinGW), use the 
autotools based build system. Run the following commands from the top 
directory (containing this file) to configure, build and install the 
library and utilities:

  $ ./configure
  $ make
  $ make install

If you downloaded the libeltako source code directly from the git
source code repository, then you need to create the configure script as
the first step:

  $ autoreconf --install

To uninstall libeltako again, run:

  $ make uninstall

Support
=======

Please send bug reports, feedback or questions via github

  https://github.com/jnevens/libeltako

or contact me directly:

  jasper.nevens@linux.com

License
=======

libeltako is free software, released under the terms of the GNU
Lesser General Public License (LGPLv2.1).

You can find a copy of the license in the file COPYING.

Usage
=====

### Physical connection
* Connect via RS-232 or USB to the FGW14 or FGW14-USB module.
* Set the rotary switch of the FGW14 to 6 (58K baud rate)
* Check with dmesg which linux device is created (example: /dev/ttyS0 or /dev/ttyUSB0)

### Eltako Deamon
eltakod [-d daemonize] [-p pidfile] [-s socket] [DEVICE]

Starts a VSB (Virtual Serial Bus) server, to provide receive and send functionality for other deamons to a single connection to the Eltako Series 14 Bus. Multiple deamons can connect to this `virtual extended eltako bus` at the same time.

### Eltako Send tool
eltako-send [-D device] [-r rorg] [-s status] ADDRESS(hex) DATA(hex)

Send individual messages to the eltako bus.

### Eltako Dump tool
eltako-dump [DEVICE]

Dumps all messages received to stdout.

Note: conflicts with eltakod while running at the same time.

Eltako message format
=====================

Each message contains 14 bytes and is constructed as follows:

```bash
A5 5A   0B    05     50 00 00 00   FE FE E2 0E     30     7C
PRE   | LEN | RORG |     DATA    |    SOURCE   | STATUS | CRC
```

Examples
========

### eltako-send

```bash
# set FUD14 100%
eltako-send -D /dev/ttyUSB0 -r 0x05 -s30 0xeeeeee00 0x0x02640009
# set FUD14 to 0% and disable
eltako-send -D /dev/ttyUSB0 -r 0x05 -s30 0xeeeeee00 0x0x02000008
# set FSR14 relais on (non blocking)
eltako-send -D /dev/ttyUSB0 -s 30 0xEEEE2000 -r0x07 0x01000009
# set FSR14 relais off (non blocking)
eltako-send -D /dev/ttyUSB0 -s 30 0xEEEE2000 -r0x07 0x01000008
# FSB blinds up 3s
eltako-send -D /dev/ttyUSB0 -s 30 0xEEEE3000 -r0x07 0xff030108
# FSB blinds down 3s
eltako-send -D /dev/ttyUSB0 -s 30 0xEEEE3000 -r0x07 0xff030208
# FSB blinds stop
eltako-send -D /dev/ttyUSB0 -s 30 0xEEEE3000 -r0x07 0xff000008
```

### eltako-dump

```bash
# eltako-dump /dev/ttyUSB0
A5 5A 0B 05 50 00 00 00 FE FE E2 0E 30 7C
message: type:butten, rorg:05, data:50 00 00 00, source 0xfefee20e, status: 30
A5 5A 0B 05 00 00 00 00 FE FE E2 0E 20 1C
message: type:butten, rorg:05, data:00 00 00 00, source 0xfefee20e, status: 20
A5 5A 0B 05 70 00 00 00 FE FE E2 0E 30 9C
message: type:butten, rorg:05, data:70 00 00 00, source 0xfefee20e, status: 30
A5 5A 0B 05 00 00 00 00 FE FE E2 0E 20 1C
message: type:butten, rorg:05, data:00 00 00 00, source 0xfefee20e, status: 20
```
