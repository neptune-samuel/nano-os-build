/*
 * General GPIO Driver Framework
 *
 * Copyright (C) 2020 LiuChuansen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * --
 * 
 */

#ifndef __GPIO_TYPE_H__
#define __GPIO_TYPE_H__
 
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/hrtimer.h>

enum
{
    GPIO_FUNCTION_DEFAULT = 0,
    GPIO_FUNCTION_LED,  // 1
    GPIO_FUNCTION_BUTTON, // 2
    GPIO_FUNCTION_BUZZER,  // 3
};


typedef struct
{
    int pin;
    int active_low;
    int dir_in;
}gpio_t;


struct gpio_info
{
    const char *name;
    int id;
    gpio_t gpio;
    int init_active;
    int do_not_request; 
    int function;
};

struct gpio_pulse
{
    spinlock_t lock;
    int enable;
    int offset;    
    u32 control;
    u32 mask; 
};

/* gpio object */
struct gpio_object
{
    struct gpio_info info;
    struct gpio_pulse pulse;
    u32 init_control;
    u32 init_mask;
    u8 with_interrupt;
    int irq;
    int irq_in_handle;
    struct kobj_attribute kobj_attr;
    struct list_head list;
};

/* oasis dev data */

struct gpio_data
{
    struct device *dev;

    /* for gpio */
    struct list_head gpios;
    struct timer_list gpio_timer;

    struct timer_list test_timer;
    
    struct kobject *gpio_kobj;
};


#endif /*__GPIO_TYPE_H__  */
