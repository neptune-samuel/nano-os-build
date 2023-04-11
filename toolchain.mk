
## toolchains
export TOOLPREFIX:=aarch64-linux-gnu-
export TOOLCHAIN_HOST:=aarch64-linux-gnu
export TOOLCHAIN_DIR:=$(PLATFORM_DIR)/toolchains/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
export TOOLCHAIN_BINDIR:=$(TOOLCHAIN_DIR)/bin
export TOOLCHAIN_SYSROOT:=$(TOOLCHAIN_DIR)/$(TOOLCHAIN_HOST)/libc
export TOOLCHAIN_SYSINCLUDE:=$(TOOLCHAIN_SYSROOT)/usr/include
export TOOLCHAIN_LIBC:=$(TOOLCHAIN_SYSROOT)/lib
export TOOLCHAIN_USRLIBC:=$(TOOLCHAIN_SYSROOT)/usr/lib

export CROSS_COMPILE=$(TOOLPREFIX)

# target toolchains
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CXX             = $(CROSS_COMPILE)g++
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
RANLIB 		    = $(CROSS_COMPILE)ranlib
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
MAKE		    = make

