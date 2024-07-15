#!/bin/sh

# 修正I2C总线连接到MCU主控导致深度传感器不可用的问题
# 0 - depth sensor
# 1 - sht3x

echo "Fixed i2c issue on depth sensor.."

#/usr/sbin/i2cdetect -y -a 0 30 50
#/usr/sbin/i2cdetect -y -a 1 20 50
echo 1 > /sys/devices/platform/extgpio/gpios/mcu_reset


echo 72 > /sys/class/gpio/export
echo 73 > /sys/class/gpio/export

echo out > /sys/class/gpio/gpio72/direction
echo 1 > /sys/class/gpio/gpio72/value
echo in > /sys/class/gpio/gpio72/direction

echo out > /sys/class/gpio/gpio73/direction
echo 1 > /sys/class/gpio/gpio73/value
echo in > /sys/class/gpio/gpio73/direction

sleep 1
echo 0 > /sys/devices/platform/extgpio/gpios/mcu_reset

