


#ifndef __GPIO_SYSFS_H__
#define __GPIO_SYSFS_H__

#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/sysfs.h>

#include "gpio_type.h"

ssize_t sysfs_set_gpio(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count);

ssize_t sysfs_get_gpio(struct kobject *kobj, struct kobj_attribute *attr, char *buf);

ssize_t show_gpio_info(struct device *dev, struct device_attribute *attr, char *buf);

int sysfs_add_gpio_node(struct gpio_data *gdata, struct gpio_object *gpio);

ssize_t sysfs_add_gpio(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count);

ssize_t sysfs_remove_gpio(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count);

#endif // __GPIO_SYSFS_H__
