#!/bin/sh

OS_IMAGE=Image 
OS_DTB=dtb
FS_OVERLAYS=rootfs-overlays.tgz 


if [ "$L4T_DIR" = "" ]; then 
  echo "L4T_DIR is not defined"
  exit 1
fi 

echo "L4T_DIR="$L4T_DIR

if [ -f $FS_OVERLAYS ]; then 
    if [ "$L4T_FS_BASE" = "" ]; then 
    echo "L4T_FS_BASE is not defined"
    exit 1
    fi   
fi 

echo "L4T_FS_BASE="$L4T_FS_BASE

#  install all

if [ -f $OS_IMAGE ];then 
  echo -n "Install kernel image.."
  cp -f $OS_IMAGE $L4T_DIR/kernel  
  echo "done"
fi 

if [ -d $OS_DTB ];then 
  echo -n "Install kernel dtbs.."
  cp -rf $OS_DTB $L4T_DIR/kernel/
  echo "done" 
fi 

if [ -f $FS_OVERLAYS ]; then 
  echo "Detect overlay fs, update rootfs"
  echo "Clean previous rootfs files"
  cd $L4T_DIR/rootfs && sudo rm -rf *
  echo -n "Install base rootfs.."
  cd $L4T_DIR/rootfs && sudo tar -jxpf $L4T_FS_BASE   
  echo "done" 
  echo -n "Install overlay files"
  sudo tar xf $FS_OVERLAYS -C $L4T_DIR/rootfs
  echo "done"
fi 

