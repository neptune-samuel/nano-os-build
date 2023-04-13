## 安装ROS2-FOXY到Nano (Ubuntu-18.04)

### 安装前准备

需要大量从github下载源码，建议使用VPN网络。


### STEP1

配置环境变量

```sh 
export ROS_PKG=ros_base
export ROS_DISTRO=foxy
export ROS_BUILD_ROOT=/opt/ros/${ROS_DISTRO}-src
export ROS_INSTALL_ROOT=/opt/ros/${ROS_DISTRO}

```

```sh 
locale
sudo apt update 
sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

sudo apt install -y --no-install-recommends curl wget gnupg2 lsb-release

sudo rm -rf /var/lib/apt/lists/*
```

### STEP2

```sh
wget --no-check-certificate https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc 
sudo apt-key add ros.asc
sudo sh -c 'echo "deb [arch=$(dpkg --print-architecture)] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" > /etc/apt/sources.list.d/ros2-latest.list'

sudo apt update  
```

### STEP3 

```sh 
sudo apt install -y --no-install-recommends build-essential cmake git libbullet-dev \
 libpython3-dev libasio-dev libtinyxml2-dev libcunit1-dev

sudo apt install -y --no-install-recommends python3-colcon-common-extensions \
 python3-flake8 python3-pip python3-pytest-cov python3-rosdep python3-setuptools \
 python3-vcstool python3-rosinstall-generator 

```

### STEP4 
```sh 
python3 -m pip install -U argcomplete flake8-blind-except \
 flake8-builtins flake8-class-newline flake8-comprehensions flake8-deprecated \
 flake8-docstrings flake8-import-order flake8-quotes pytest-repeat \
 pytest-rerunfailures pytest
```

### STEP5

安装yaml-cpp (最新的文件系统已安装yaml-cpp这个库，可跳过)

```sh 
git clone --branch yaml-cpp-0.6.0 https://github.com/jbeder/yaml-cpp yaml-cpp-0.6 
cd yaml-cpp-0.6 
mkdir build 
cd build 
cmake -DBUILD_SHARED_LIBS=ON .. 
make -j$(nproc) 
sudo cp libyaml-cpp.so.0.6.0 /usr/lib/aarch64-linux-gnu/ 
sudo ln -s /usr/lib/aarch64-linux-gnu/libyaml-cpp.so.0.6.0 /usr/lib/aarch64-linux-gnu/libyaml-cpp.so.0.6

```

### STEP6

方法一: 使用已下载源码 ros2-foxy-base-src.tgz 
```sh 
sudo mkdir /opt/ros/foxy-src 
sudo tar xf ros2-foxy-base-src.tgz -C /opt/ros/foxy-src/
```

方法二: 使用网络下载 

```sh 
sudo mkdir -p ${ROS_BUILD_ROOT}/src 
cd ${ROS_BUILD_ROOT}
sudo sh -c "rosinstall_generator --deps --rosdistro ${ROS_DISTRO} ${ROS_PKG} launch_xml launch_yaml example_interfaces > ros2.${ROS_DISTRO}.${ROS_PKG}.rosinstall" 
sudo cat ros2.${ROS_DISTRO}.${ROS_PKG}.rosinstall && vcs import src < ros2.${ROS_DISTRO}.${ROS_PKG}.rosinstall

# download unreleased packages     
sudo sh -c "git clone --branch ros2 https://github.com/Kukanani/vision_msgs ${ROS_BUILD_ROOT}/src/vision_msgs && \
    git clone --branch ${ROS_DISTRO} https://github.com/ros2/demos demos && \
    cp -r demos/demo_nodes_cpp ${ROS_BUILD_ROOT}/src && \
    cp -r demos/demo_nodes_py ${ROS_BUILD_ROOT}/src && \
    rm -r -f demos"

```

最新的包多了一个rviz这个包，目前暂不支持这个，衔删除这个

```sh
sudo rm -rf src/vision_msgs/vision_msgs_rviz_plugins
```


### STEP7 

编译，如果出问题，请参考 issue_fixed.md

```sh 
# build it!
sudo mkdir -p ${ROS_INSTALL_ROOT}
# sudo required to write build logs
# 如果出问题，请参考 issue_fixed.md ,再次执行这个命令 
sudo colcon build --merge-install --install-base ${ROS_INSTALL_ROOT}

# Using " expands environment variable immediately
echo "source $ROS_INSTALL_ROOT/setup.bash" >> ~/.bashrc 
# echo "source /usr/share/colcon_cd/function/colcon_cd.sh" >> ~/.bashrc
# echo "export _colcon_cd_root=~/ros2_install" >> ~/.bashrc

```

