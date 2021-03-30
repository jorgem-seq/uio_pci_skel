# SPDX-License-Identifier: GPL-2.0

obj-m += uio_pci_skel.o

KVERSION = $(shell uname -r)
CURR_PWD = $(shell pwd)

all: module

module:
	make -C /lib/modules/$(KVERSION)/build M=$(CURR_PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(CURR_PWD) clean
