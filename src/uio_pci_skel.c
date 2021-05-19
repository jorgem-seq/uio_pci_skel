// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>

MODULE_AUTHOR("Jorge Maidana <jorgem.seq@gmail.com>");
MODULE_DESCRIPTION("UIO PCI Skeleton Driver");
MODULE_LICENSE("GPL v2");

#define DRIVER_VERSION				"mem0-noirqh"
#define UIO_SKEL_BAR0				0
#define UIO_SKEL_MEM0				0

/*** Add a valid PCI ID ***/
#define PCI_VENDOR_ID_UIO_SKEL			0x0000
#define PCI_DEVICE_ID_UIO_SKEL_PCI_DEV01	0x0000

#if !PCI_VENDOR_ID_UIO_SKEL || !PCI_DEVICE_ID_UIO_SKEL_PCI_DEV01
#error Add a valid PCI ID
#endif

/*** Add interrupt handler code ***/
static irqreturn_t uio_pci_skel_irq_handler(int __maybe_unused irq,
					    struct uio_info __maybe_unused *dev_info)
{
#warning Add interrupt handler code

	return IRQ_NONE;
}

static int uio_pci_skel_setup_mem(struct pci_dev *pdev, struct uio_info *info,
				  int mem, int bar)
{
	info->mem[mem].addr = pci_resource_start(pdev, bar);
	if (!info->mem[mem].addr)
		return -ENXIO;
	info->mem[mem].internal_addr = pci_ioremap_bar(pdev, bar);
	if (!info->mem[mem].internal_addr)
		return -ENODEV;
	info->mem[mem].size = pci_resource_len(pdev, bar);
	info->mem[mem].memtype = UIO_MEM_PHYS;
	return 0;
}

static int uio_pci_skel_probe(struct pci_dev *pdev,
			      const struct pci_device_id __maybe_unused *pci_id)
{
	struct uio_info *info;
	int ret;

	if (pci_enable_device(pdev))
		return -ENODEV;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		ret = -ENOMEM;
		dev_err(&pdev->dev, "can't allocate memory\n");
		goto disable_device;
	}

	ret = pci_request_regions(pdev, KBUILD_MODNAME);
	if (ret) {
		dev_err(&pdev->dev, "can't request memory regions\n");
		goto disable_device;
	}

	ret = uio_pci_skel_setup_mem(pdev, info, UIO_SKEL_MEM0, UIO_SKEL_BAR0);
	if (ret) {
		dev_err(&pdev->dev, "can't configure MEM %d\n", UIO_SKEL_MEM0);
		goto free_pci_regions;
	}

	info->name = KBUILD_MODNAME;
	info->version = DRIVER_VERSION;
	info->irq = pdev->irq;
	info->irq_flags = IRQF_SHARED;
	info->handler = uio_pci_skel_irq_handler;

	ret = uio_register_device(&pdev->dev, info);
	if (ret) {
		dev_err(&pdev->dev, "can't register UIO device\n");
		goto free_mem0;
	}

	pci_set_drvdata(pdev, info);

	pr_info("%s: mem0: 0x%p, irq %u\n", info->name,
		info->mem[UIO_SKEL_MEM0].internal_addr, pdev->irq);
	return 0;

free_mem0:
	iounmap(info->mem[UIO_SKEL_MEM0].internal_addr);
free_pci_regions:
	pci_release_regions(pdev);
disable_device:
	pci_disable_device(pdev);
	return ret;
}

static void uio_pci_skel_remove(struct pci_dev *pdev)
{
	struct uio_info *info = pci_get_drvdata(pdev);

	uio_unregister_device(info);
	iounmap(info->mem[UIO_SKEL_MEM0].internal_addr);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static const struct pci_device_id uio_pci_skel_tbl[] = {
	{ PCI_DEVICE_DATA(UIO_SKEL, PCI_DEV01, 0) },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, uio_pci_skel_tbl);

static struct pci_driver uio_pci_skel_driver = {
	.name		= KBUILD_MODNAME,
	.id_table	= uio_pci_skel_tbl,
	.probe		= uio_pci_skel_probe,
	.remove		= uio_pci_skel_remove,
};

module_pci_driver(uio_pci_skel_driver);
