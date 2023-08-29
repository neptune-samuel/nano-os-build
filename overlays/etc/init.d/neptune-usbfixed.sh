#!/bin/sh


## only accept start
case "$1" in
  start) ;;
  stop|restart|force-reload) exit 0 ;;
  *) echo "Usage: $0 {start|stop|restart|force-reload}" >&2; exit 1 ;;
esac

# Fix compatibility issue on backboard P3541 with P3348

echo "fxied usb issue with P3541 bottom-board"

echo 66 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio66/direction
echo 1 > /sys/class/gpio/gpio66/value

