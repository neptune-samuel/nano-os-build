

CONFIG_OSMSG_NETLINK=y

obj-m := extgpio.o
extgpio-objs := driver_main.o lowlevel.o sysfs.o irq_handle.o

ccflags-$(CONFIG_EXTGPIO_USE_DTS) += -DCONFIG_EXTGPIO_USE_DTS
ccflags-$(CONFIG_OSMSG_NETLINK) += -DCONFIG_OSMSG_NETLINK -I$(DRIVER_DIR)/osmsg
ccflags-$(CONFIG_SOC_GPIO_NAME_SSD2X2) += -DCONFIG_SOC_GPIO_NAME_SSD2X2

install:
	install -m 755 extgpio.ko $(DESTDIR)/lib/modules/
