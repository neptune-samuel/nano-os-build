#!/bin/sh

case "$1" in
  start) ;;
  stop|restart|force-reload) exit 0 ;;
  *) echo "Usage: $0 {start|stop|restart|force-reload}" >&2; exit 1 ;;
esac

echo "==> Initialize extgpio.."

## load extgpio
insmod /lib/modules/osmsg.ko
insmod /lib/modules/extgpio.ko

EXTGPIO_EXPORT=/sys/devices/platform/extgpio/export

if [ ! -f ${EXTGPIO_EXPORT} ]; then
    echo "==> Unabled to insmod extgpio module!!!"
    exit 1
fi

chown -R root:gpio /sys/devices/platform/extgpio
chmod 0775 /sys/devices/platform/extgpio/gpios
chmod 0664 /sys/devices/platform/extgpio/gpio_info
chmod 0664 /sys/devices/platform/extgpio/export
chmod 0664 /sys/devices/platform/extgpio/unexport
