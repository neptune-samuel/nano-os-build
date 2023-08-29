


#ifndef __EXTGPIO_IRQHANDLE_H__
#define __EXTGPIO_IRQHANDLE_H__

#include <linux/string.h>
#include <linux/ctype.h>

#include "gpio_type.h"

int extgpio_with_interrupt( struct gpio_object *obj);

int extgpio_set_interrupt(struct gpio_data *gdata, struct gpio_object *obj);

void extgpio_unset_interrupt(struct gpio_data *gdata, struct gpio_object *obj);


#endif // __EXTGPIO_IRQHANDLE_H__
