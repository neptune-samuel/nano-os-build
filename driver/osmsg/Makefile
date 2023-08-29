
obj-m := osmsg.o
osmsg-objs := osmsg_main.o osmsg_netlink.o osmsg_api.o

ccflags-y += -DCONFIG_OSMSG_NETLINK_TYPE=25

install:
	install -m 755 osmsg.ko $(DESTDIR)/lib/modules/

