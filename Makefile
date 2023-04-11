PLATFORM_DIR=$(shell pwd)
IMAGE_DIR=$(PLATFORM_DIR)/images
MODULES_DIR=$(IMAGE_DIR)/modules
MAKE_JOBS?=-j$(shell nproc)
CLEAN?=n

## uboot parameters
export UBOOT_DIR=$(PLATFORM_DIR)/u-boot
UBOOT_CONFIG=p2371-2180_defconfig
#UBOOT_CONFIG=p2771-0000-500_defconfig

## kernel parameters
export KERNEL_DIR=$(PLATFORM_DIR)/kernel/kernel-4.9
KERNEL_ARCH=arm64
KERNEL_CONFIG=tegra_defconfig 
KERNEL_TARGET=zImage
KERNEL_INSTALL_PATH=$(IMAGE_DIR)/kernel

## DTBS for jetson nano
# tegra210-p3448-0003-p3542-0000.dtb
# tegra210-p3448-0003-p3542-0000-hdmi-dsi.dtb
# tegra210-p3448-0002-p3449-0000-b00.dtb
# tegra210-p3448-0000-p3449-0000-b00.dtb
# tegra210-p3448-0000-as-p3448-0003.dtb
# tegra210-p3448-0000-p3449-0000-a00.dtb
# tegra210-p3448-0000-p3449-0000-a02-hdmi-dsi.dtb
# tegra210-p3448-0000-p3449-0000-a02.dtb
# tegra210-p3448-0002-p3449-0000-a02.dtb
# tegra210-p3448-0000-p3449-0000-b00-hdmi-dsi.dtb

## load toolchain rules
include toolchain.mk 

ifneq ("$(TOOLCHAIN_BINDIR)","")
export PATH:=$(TOOLCHAIN_BINDIR):${PATH}
endif

ifneq ("$(UBOOT_DIR)","")
export PATH:=$(UBOOT_DIR)/tools:${PATH}
endif

ifdef _c 
CLEAN=$(_c)
endif 

ifdef _j 
MAKE_JOBS:=$(_j)
endif 

.PHONY:help 

help:
	@echo ""
	@echo "======== Makefile Help ============================"
	@echo " options:"
	@echo "  _c=[y|n] : enable clean build for target"
	@echo "  _j=n     : set build jobs"
	@echo ""

## for uboot 
.PHONY: uboot_config uboot_clean uboot_build uboot_install uboot

uboot_config:
	$(MAKE) -C $(UBOOT_DIR) CROSS_COMPILE=$(CROSS_COMPILE) $(UBOOT_CONFIG)

uboot_build: 
	$(MAKE) -C $(UBOOT_DIR) $(MAKE_JOBS) CROSS_COMPILE=$(CROSS_COMPILE) --output-sync=target

uboot_clean:
	$(MAKE) -C $(UBOOT_DIR) clean

uboot_install:
	mkdir -p $(IMAGE_DIR)
	cd $(UBOOT_DIR) && cp -f u-boot.bin $(IMAGE_DIR)/


uboot-$(CLEAN) = uboot_clean
uboot-y += uboot_config
uboot-y += uboot_build 
uboot-y += uboot_install

uboot: $(uboot-y) 


## Linux ## 
.PHONY: linux_check linux_clean linux_config linux_save_config linux_dtbs linux_build linux_modules_build linux_menuconfig linux


$(KERNEL_INSTALL_PATH):
	mkdir -p $(KERNEL_INSTALL_PATH)

linux_check: $(KERNEL_INSTALL_PATH)

KERNEL_OPTIONS=ARCH=$(KERNEL_ARCH) LOCALVERSION="-tegra" INSTALL_PATH=$(KERNEL_INSTALL_PATH) CROSS_COMPILE=$(CROSS_COMPILE)

linux_clean: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(KERNEL_OPTIONS) mrproper

linux_config: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(KERNEL_OPTIONS) $(KERNEL_CONFIG)

linux_save_config:
	cd $(KERNEL_DIR) && cp .config arch/$(KERNEL_ARCH)/configs/$(KERNEL_CONFIG)

# Kernel DTB build targets
linux_dtbs: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(KERNEL_OPTIONS) dtbs

linux_build: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(MAKE_JOBS) $(KERNEL_OPTIONS) --output-sync=target $(KERNEL_TARGET)

linux_modules_build: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(MAKE_JOBS) $(KERNEL_OPTIONS) --output-sync=target modules

.PHONY: linux_images_install linux_dtbs_install linux_modules_install

# linux_images_install:linux_check
# 	@echo "Install $(KERNEL_DIR)/arch/$(KERNEL_ARCH)/boot/$(KERNEL_TARGET) to $(IMAGE_DIR)"
# 	cp -f $(KERNEL_DIR)/arch/$(KERNEL_ARCH)/boot/$(KERNEL_TARGET) $(IMAGE_DIR)/

# linux_dtbs_install:linux_check
	

# linux_modules_install:linux_check
# 	$(MAKE) -C $(KERNEL_DIR) $(KERNEL_OPTIONS) \
# 	INSTALL_MOD_PATH=$(MODULES_DIR) modules_install

#linux_install: linux_images_install linux_dtbs_install linux_modules_install

linux-$(CLEAN) = linux_clean
linux-y += linux_config
linux-y += linux_build
linux-y += linux_dtbs
linux-n += linux_modules_build
#linux-y += linux_install 

linux: $(linux-y)

linux_menuconfig:
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) menuconfig		


.PHONY: dump 
dump:
	@echo "CLEAN="$(CLEAN)
	@echo "MAKE_JOBS="$(MAKE_JOBS)
	$(CC) -v 

