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
  extra=${4:-}

  echo $name $number $mode $extra > ${EXTGPIO_EXPORT}
  chown root:gpio ${EXTGPIO_GPIOS}/$name
  chmod 0664 ${EXTGPIO_GPIOS}/$name
}


add_gpio sys_led 39 out
add_gpio mcu_reset 62 out
add_gpio mcu_upgrade 66 out
add_gpio plc_state 65 in 
add_gpio power_down 216 in active-low

# enable irq 
if [ -f ${EXTGPIO_GPIOS}/power_down ]; then 
echo "==> enable power_down interrupt"
echo irq_on > ${EXTGPIO_GPIOS}/power_down
fi 

# turn sysled flash
echo 0xaaaaaaaa > ${EXTGPIO_GPIOS}/sys_led
