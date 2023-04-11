## NANO板内核编译工程

当前代码来自于以下链接：

https://developer.nvidia.com/embedded/linux-tegra-r3273

### 环境准备

for uboot
> sudo apt instal bison flex bc 

for kernel
> sudo apt install libncurses5-dev

### 安装工具链

当前工程不包含工具链，需要手动选择工具链

安装方法

> cd toolchains 
> git clone https://github.com/neptune-samuel/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu.git

### 编译u-boot

> make uboot 

### 编译内核 

> make linux 

#### DTS修改
请注意：
官方并没有使用kernel/arch/arm64/boot/dts目录中的DTS文件，而是使用外面的hardware目录中的DTS文件，在内核的scripts/Kbuild.include文件中，引用了这个目录。

```sh

scripts/Kbuild.include:tegra-dtstree = $(NV_BUILD_KERNEL_DTS_ROOT)
scripts/Kbuild.include:tegra-dtstree = $(srctree)/../../hardware/nvidia
scripts/Kbuild.include:tegra-root-dtstree = $(subst ^$(realpath $(tegra-dtstree)/../..)/,,^$(realpath $(srctree)/arch/arm64/boot/dts))

```

### 输出的文件 

生成的文件在images目录

