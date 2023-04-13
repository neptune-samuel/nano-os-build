PLATFORM_DIR=$(shell pwd)
IMAGE_DIR=$(PLATFORM_DIR)/images
MODULES_DIR=$(IMAGE_DIR)/modules
ROOTFS_DIR=$(PLATFORM_DIR)/rootfs
RELEASED_DIR=$(PLATFORM_DIR)/released
MAKE_JOBS?=-j$(shell nproc)
CLEAN?=n
MODULES?=n

## build time info
NOW_SECOND=$(shell date -d "now" +"%s")
V_DATE=$(shell date +"%Y%m%d" -d @$(NOW_SECOND))

## uboot parameters
export UBOOT_DIR=$(PLATFORM_DIR)/u-boot
UBOOT_CONFIG=p2371-2180_defconfig
#UBOOT_CONFIG=p2771-0000-500_defconfig

## kernel parameters
export KERNEL_DIR=$(PLATFORM_DIR)/kernel/kernel-4.9
KERNEL_ARCH=arm64
KERNEL_CONFIG=tegra_defconfig 
KERNEL_TARGET=zImage
KERNEL_INSTALL_PATH=$(IMAGE_DIR)
KERNEL_DTBS=tegra210-p3448-0002-p3449-0000-b00.dtb

KERNEL_DTB_INSTALL_PATH=$(KERNEL_INSTALL_PATH)/dtb
KERNEL_MODULES_INSTALL_PATH=$(KERNEL_INSTALL_PATH)/modules

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

## 根文件系统 
PREBUILT_ROOTFS:=zips/Tegra_Linux_Sample-Root-Filesystem_R32.7.3_aarch64.tbz2


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
	@echo " -- Show environments --"
	@echo " make dump "
	@echo ""
	@echo " -- Build uboot --"
	@echo " make [_c=y] uboot "
	@echo ""
	@echo " -- Build linux --"
	@echo " make [_c=y] [MODULES=y] linux"
	@echo "  - MODULES=y -- build kernel modules "
	@echo ""
	@echo " -- Linux Menuconfig --"
	@echo " make linux_menuconfig"
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

$(KERNEL_DTB_INSTALL_PATH):
	mkdir -p $(KERNEL_DTB_INSTALL_PATH)

$(KERNEL_MODULES_INSTALL_PATH):
	mkdir -p $(KERNEL_MODULES_INSTALL_PATH)

linux_check: $(KERNEL_INSTALL_PATH) $(KERNEL_DTB_INSTALL_PATH) $(KERNEL_MODULES_INSTALL_PATH)

KERNEL_OPTIONS=ARCH=$(KERNEL_ARCH) LOCALVERSION="-tegra" INSTALL_PATH=$(KERNEL_INSTALL_PATH) CROSS_COMPILE=$(CROSS_COMPILE)
#KERNEL_OPTIONS=ARCH=$(KERNEL_ARCH) LOCALVERSION="-tegra" CROSS_COMPILE=$(CROSS_COMPILE)

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

linux_modules: linux_check
	$(MAKE) -C $(KERNEL_DIR) $(MAKE_JOBS) $(KERNEL_OPTIONS) --output-sync=target modules

linux_modules_install:linux_check
	$(MAKE) -C $(KERNEL_DIR) $(KERNEL_OPTIONS) INSTALL_MOD_PATH=$(KERNEL_MODULES_INSTALL_PATH) modules_install

.PHONY: linux_images_install linux_dtbs_install linux_modules_install

linux_install:linux_check
	@echo "Install $(KERNEL_DIR)/arch/$(KERNEL_ARCH)/boot/Image to $(KERNEL_INSTALL_PATH)"
	cp -f $(KERNEL_DIR)/arch/$(KERNEL_ARCH)/boot/Image $(KERNEL_INSTALL_PATH)/
	cd $(KERNEL_DIR)/arch/$(KERNEL_ARCH)/boot/dts/ && cp $(KERNEL_DTBS) $(KERNEL_DTB_INSTALL_PATH)
	
linux-$(CLEAN) = linux_clean
linux-y += linux_config
linux-y += linux_build
linux-y += linux_dtbs
linux-y += linux_install 
linux-$(MODULES) += linux_modules 
linux-$(MODULES) += linux_modules_install

linux: $(linux-y)

linux_menuconfig:
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(KERNEL_ARCH) menuconfig		


.PHONY: rootfs rootfs_overlays

FS_VERSION:=v$(V_DATE)
ROOTFS_OVERLAYS:=$(IMAGE_DIR)/rootfs-overlays.tgz 

rootfs_overlays:
	rm -rf $(ROOTFS_DIR) overlays.tgz 
	mkdir -p $(ROOTFS_DIR)
	cd overlays && tar czf ../overlays.tgz * && cd .. && tar xvf overlays.tgz -C $(ROOTFS_DIR) && rm -f overlays.tgz 
	echo $(FS_VERSION) > $(ROOTFS_DIR)/etc/neptune_version
	cd $(ROOTFS_DIR) && tar czf $(ROOTFS_OVERLAYS) *
	echo "Build rootfs overlay ok"

rootfs: rootfs_overlays


.PHONY: release 

RELEASED_NAME:=neptune-nano-$(FS_VERSION)
CURRENT_RELEASED_DIR:=$(RELEASED_DIR)/$(RELEASED_NAME)
CURRENT_RELEASED_TGZ:=$(RELEASED_DIR)/$(RELEASED_NAME).tgz 

release:
	rm -rf $(CURRENT_RELEASED_DIR)/* $(CURRENT_RELEASED_TGZ)
	mkdir -p $(CURRENT_RELEASED_DIR)
	cd $(IMAGE_DIR) && tar czf ../images.tgz * && cd .. && tar xvf images.tgz -C $(CURRENT_RELEASED_DIR) && rm -f images.tgz 
	cp -f CHANGELOG.md $(CURRENT_RELEASED_DIR)/
	cp -f scripts/install_to_l4t.sh $(CURRENT_RELEASED_DIR)/install.sh 
	cd $(RELEASED_DIR) && tar czf $(CURRENT_RELEASED_TGZ) $(RELEASED_NAME)
	@echo "Release file: "$(RELEASED_NAME).tgz " is ready"

.PHONY: dump 
dump:
	@echo "CLEAN="$(CLEAN)
	@echo "MAKE_JOBS="$(MAKE_JOBS)
	@echo "UBOOT_DIR="$(UBOOT_DIR)
	@echo "UBOOT_CONFIG="$(UBOOT_CONFIG)
	@echo "KERNEL_DIR="$(KERNEL_DIR)
	@echo "KERNEL_ARCH=="$(KERNEL_ARCH)
	@echo "KERNEL_CONFIG="$(KERNEL_CONFIG)
	@echo "KERNEL_DTBS="$(KERNEL_DTBS)
	@echo ""
	@echo "== gcc toolchain info =="
	@$(CC) -v 

