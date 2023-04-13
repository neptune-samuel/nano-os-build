## 如何制作NANO系统镜像 

### 组件

UBOOT
内核： 内核镜像，DTB，内核模块 
OVERLAY替换文件 

基础文件系统: NVIDIA 官方发布
SDKMNANGER安装环境 

rm -rf image
下面的编译选项可选 
make linux
make uboot
make rootfs 

发布，生成相应的压缩包，放在released目录
make release

git tag 

将压缩包放入烧录环境

解压，执行安装脚本

执行烧录命令



### 重做镜像

- 编译内核(内核镜像，DTB，内核模块)

- 编译文件系统 

- 安装到烧录目录 

- 烧录设备

- 启动设备,进行首次配置，设定用户名和密码 

- 安装ROS2 

- 使用烧录工具备份镜像

- 发布该镜像

### 小版本更新

如果版本更新满足以下条件：
  - 内核改动
  - DTB改动
  - 文件系统改动 

通过实现补丁包的方式升级，补丁包提供自升级的脚本供使用。

补丁包: NeptuneOne-ROS-V20230510.zip

### 说明

#### 编译内核(内核镜像，DTB，内核模块)


输出文件:
 image/Image 
 image/dtb/*.dtb 
 image/modules/*


#### 编译文件系统

解压基础文件系统
安装overlays 
安装内核模块 

输出文件:
  image/rootfs.tgz 

#### 编译文件系统 

#### 安装到烧录目录 
  复制内核，文件系统等到烧录工具目录
  执行 apply_binaries.sh 

#### 烧录设备

#### 启动设备,进行首次配置，设定用户名和密码 

#### 安装ROS2 

TODO：使用qemu直接在系统安装好 

#### 使用烧录工具备份镜像



#### 发布该镜像
