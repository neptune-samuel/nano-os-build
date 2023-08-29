#!/bin/sh

echo "==> Initialize gpios for neptune-n1 hardware-V2.0"

EXTGPIO_DIR=/sys/devices/platform/extgpio
EXTGPIO_EXPORT=${EXTGPIO_DIR}/export
EXTGPIO_GPIOS=${EXTGPIO_DIR}/gpios

if [ ! -f ${EXTGPIO_EXPORT} ]; then
    echo "==> extgpio module not exist!!!"
    exit 1
fi

add_gpio()
{
  name=$1
  number=$2
  mode=$3

  echo $name $number $mode > ${EXTGPIO_EXPORT}
  chown root:gpio ${EXTGPIO_GPIOS}/$name
  chmod 0664 ${EXTGPIO_GPIOS}/$name
}


add_gpio sysled 39 out
add_gpio mcureset 62 out
add_gpio mcuupgrade 66 out

# turn sysled flash
echo 0xaaaaaaaa > ${EXTGPIO_GPIOS}/sysled
