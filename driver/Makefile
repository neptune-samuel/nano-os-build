

include $(DRIVER_DIR)/Kbuild

DRIVER_BUILD=$(foreach obj,$(driver-y),drv-build-$(obj))
DRIVER_INSTALL=$(foreach obj,$(driver-y),drv-install-$(obj))
DRIVER_CLEAN=$(foreach obj,$(driver-y),drv-clean-$(obj))

driver_build : 
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(DRIVER_DIR) modules

driver_install : $(DRIVER_INSTALL)

driver_clean : $(DRIVER_CLEAN)
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(DRIVER_DIR) clean


## common targets
drv-build-%:
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(DRIVER_DIR)/$* modules

drv-install-%:
	$(MAKE) -C $(DRIVER_DIR)/$* DESTDIR=$(STAGING_PREFIX) install

drv-clean-%:
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(DRIVER_DIR)/$* clean


## for all rules
drvbuild_$(CLEAN_BUILD) = drv-clean-
drvbuild_y += drv-build-
drvbuild_y += drv-install-

define driver-simple-build
for t in $(addsuffix $@,$(drvbuild_y)) ; do $(MAKE) $$t; done
endef

$(driver-y):
	$(driver-simple-build)

