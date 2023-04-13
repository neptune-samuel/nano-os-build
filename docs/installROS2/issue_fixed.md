
## 参考 installROS2.sh

### 环境变量
> export ROS_PKG=ros_base
> export ROS_DISTRO=foxy
> export ROS_BUILD_ROOT=/opt/ros/${ROS_DISTRO}-src
> 
> export ROS_INSTALL_ROOT=/opt/ros/${ROS_DISTRO}


### 问题1 

执行这句话时，出现以下问题
rosdep install --from-paths src --ignore-src --rosdistro ${ROS_DISTRO} -y --skip-keys "console_bridge fastcdr fastrtps rti-connext-dds-5.3.1 urdfdom_headers qtgui" 


```sh 

updated cache in /home/test/.ros/rosdep/sources.cache
ERROR: the following packages/stacks could not have their rosdep keys resolved
to system dependencies:
vision_msgs_rviz_plugins: No definition of [rviz_rendering] for OS version [bionic]

```

其实是因为最新的发布版本多一个包 :
vision_msgs_rviz_plugins        src/vision_msgs/vision_msgs_rviz_plugins        (ros.ament_cmake)

在src目录中，将它删除即可。

### 问题2 

```sh 
You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b <new-branch-name>

HEAD is now at 19ab075 Release 0.7-1
CMake Error at CMakeLists.txt:7 (cmake_minimum_required):
  CMake 3.11 or higher is required.  You are running version 3.10.2


make[2]: *** [foo_mem-ext-prefix/src/foo_mem-ext-stamp/foo_mem-ext-configure] Error 1
make[1]: *** [CMakeFiles/foo_mem-ext.dir/all] Error 2
make: *** [all] Error 2
---
Failed   <<< foonathan_memory_vendor [14.1s, exited with code 2]
Aborted  <<< osrf_pycommon [4.25s]

```

每次执行colcon build时，会在当前目录创建一个build目录

查找发现如下文件：

build/foonathan_memory_vendor/foo_mem-ext-prefix/src/foo_mem-ext/CMakeLists.txt:cmake_minimum_required(VERSION 3.11)

将它改成3.10即可。


### 问题3 
包libyaml_vendor 编译出错

```sh 

HEAD is now at 2c891fc Changes for v0.2.5 release
error: can't open patch '/opt/ros/foxy-src/src/libyaml_vendor/0001-Install-yaml.h-to-INSTALL_INCLUDE_DIR.patch': No such file or directory
make[2]: *** [libyaml-0.2.5-prefix/src/libyaml-0.2.5-stamp/libyaml-0.2.5-patch] Error 128
make[1]: *** [CMakeFiles/libyaml-0.2.5.dir/all] Error 2
make: *** [all] Error 2
---
Failed   <<< libyaml_vendor [15.4s, exited with code 2]

```

1>
在build 之前，执行一下这句话
sudo rm -f ${ROS_BUILD_ROOT}/src/libyaml_vendor/CMakeLists.txt
sudo wget --no-check-certificate https://raw.githubusercontent.com/ros2/libyaml_vendor/master/CMakeLists.txt -P ${ROS_BUILD_ROOT}/src/libyaml_vendor/

2>
还得将0001-Install-yaml.h-to-INSTALL_INCLUDE_DIR.patch 这个文件复制进来才行。 

以上两种做法还是不行，还是得把最新的libyaml 直接替换整个目录才行。



### 问题4 
提示cmake 无法找到Qt5的cmake。。。

sudo apt install qt5-default

```sh
CMake Error at CMakeLists.txt:19 (find_package):
  By not providing "FindQt5.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "Qt5", but
  CMake did not find one.

  Could not find a package configuration file provided by "Qt5" with any of
  the following names:

    Qt5Config.cmake
    qt5-config.cmake

  Add the installation prefix of "Qt5" to CMAKE_PREFIX_PATH or set "Qt5_DIR"
  to a directory containing one of the above files.  If "Qt5" provides a
  separate development package or SDK, be sure it has been installed.


---
Failed   <<< vision_msgs_rviz_plugins [5.61s, exited with code 1]
Aborted  <<< message_filters [5min 17s]
Aborted  <<< rosbag2_cpp [3min 42s]
Aborted  <<< rclcpp_lifecycle [3min 57s]

Summary: 177 packages finished [1h 9min 58s]
```

### 问题5

```sh
CMake Error at CMakeLists.txt:30 (find_package):
  By not providing "Findrviz_common.cmake" in CMAKE_MODULE_PATH this project
  has asked CMake to find a package configuration file provided by
  "rviz_common", but CMake did not find one.

  Could not find a package configuration file provided by "rviz_common" with
  any of the following names:

    rviz_commonConfig.cmake
    rviz_common-config.cmake

  Add the installation prefix of "rviz_common" to CMAKE_PREFIX_PATH or set
  "rviz_common_DIR" to a directory containing one of the above files.  If
  "rviz_common" provides a separate development package or SDK, be sure it
  has been installed.


---
Failed   <<< vision_msgs_rviz_plugins [6.81s, exited with code 1]
```
将这个包删除后，不存在这个问题了

