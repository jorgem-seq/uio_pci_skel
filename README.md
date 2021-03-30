## UIO PCI Skeleton Driver for Linux 4.19+

### Loading the module

Under ./src run:

```sh
$ make
$ sudo modprobe uio
$ sudo insmod uio_pci_skel.ko
```

### Unloading the module

```sh
$ sudo rmmod -f uio_pci_skel
```
