
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
 * Created by LiuChuansen 20190809
 */
 
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>

#include "gpio_type.h"
#include "lowlevel.h"

/**
 * @brief 定义通用的GPIO名称
 * 
 */
#ifdef CONFIG_SOC_GPIO_NAME_8BIT_ABCD

static const char * pin_name_abcd(int pin)
{

#define GPIO_NAMES "A0\0" \
            "A1\0" \
            "A2\0" \
            "A3\0" \
            "A4\0" \
            "A5\0" \
            "A6\0" \
            "A7\0" \
            "B0\0" \
            "B1\0" \
            "B2\0" \
            "B3\0" \
            "B4\0" \
            "B5\0" \
            "B6\0" \
            "B7\0" \
            "C0\0" \
            "C1\0" \
            "C2\0" \
            "C3\0" \
            "C4\0" \
            "C5\0" \
            "C6\0" \
            "C7\0" \
            "D0\0" \
            "D1\0" \
            "D2\0" \
            "D3\0" \
            "D4\0" \
            "D5\0" \
            "D6\0" \
            "D7\0\0"

    const char *p = GPIO_NAMES;
    int id = 0;
   
    do 
    {
        if (pin == id)
        {
            return p;
        }

        p = p + strlen(p) + 1;
        id ++;
    }
    while(*p && (id < 32));

    return "";
}

#endif //

#ifdef CONFIG_SOC_GPIO_NAME_SSD2X2
#include "gpio_name_ssd2x2.c"
#endif 

/**
 * @brief 返回gpio的名称
 * 
 * @param gpio 
 * @param buffer 
 * @return const char* 
 */
const char *extgpio_pin_name(int gpio, char *buffer)
{
    if (!gpio_is_valid(gpio))
    {
        return "n/a";
    }

#ifdef CONFIG_SOC_GPIO_NAME_SSD2X2
    if (gpio < sizeof(gpio_names) / sizeof(gpio_names[0]))
    {
        sprintf(buffer, "%s", gpio_names[gpio]);
    }
    else 
    {
        sprintf(buffer, "gpio<%d>", gpio);
    }
#else 

    int group, pin;
    char group_name[32];

    group = gpio / 32;
    pin = gpio % 32;
    sprintf(group_name, "gpio%d", group);

 #ifdef CONFIG_SOC_GPIO_NAME_OS_CHIP   
    struct gpio_chip *gchip;

    gchip = gpio_to_chip(gpio);

    if (gchip)
    {
        pin = gpio - gchip->base;
        sprintf(group_name, "%s", gchip->label);
    }
#endif 

    #ifdef CONFIG_SOC_GPIO_NAME_8BIT_ABCD
    sprintf(buffer, "%s_%s", group_name, pin_name_abcd(pin));
    #else
    sprintf(buffer, "%s_%d", group_name, pin);
    #endif 

#endif 

    return buffer;
}


/**
 * @brief 配置GPIO的状态
 * 
 * @param gpio 
 * @param value 
 */
void extgpio_set(gpio_t *gpio, int value)
{
    char buffer[32];

    // treat as output 
    if (gpio->dir_in)
    {
        printk(KERN_INFO ": warning, trying to set input gpio(%s)\n", extgpio_pin_name(gpio->pin, buffer));
        return ;
    }
    gpio_direction_output(gpio->pin, gpio->active_low ? !value : value);
}

/**
 * @brief 读取GPIO的值
 * 
 * @param gpio 
 * @return int 
 */
int extgpio_get(gpio_t *gpio)
{
    int value = gpio_get_value(gpio->pin);
    if (value < 0)
    {
        return value;
    }
    return gpio->active_low ? !value : value;
}



/**
 * @brief GPIO初始化脉冲控制
 * 
 * @param pulse 
 */
void extgpio_pulse_init(struct gpio_pulse *pulse)
{
    pulse->control = 0;
    pulse->mask = 0;
    pulse->enable = 0;
    pulse->offset = 0;

    /* init a lock */
    spin_lock_init(&pulse->lock);
}

/**
 * @brief GPIO脉冲控制设置
 * 
 * @param og GPIO的对象
 * @param control 控制字
 * @param mask 控制掩码
 */
void extgpio_pulse_set(struct gpio_object *og, u32 control, u32 mask)
{
    spin_lock(&og->pulse.lock);

    og->pulse.control = control;
    og->pulse.mask = mask;
    og->pulse.enable = 1;
    og->pulse.offset = 0;

    if (control == 0)
    {
        extgpio_set(&og->info.gpio, 0);
    }

    spin_unlock(&og->pulse.lock);
}

/**
 * @brief GPIO取消脉冲控制
 * 
 * @param og 
 */
void extgpio_pulse_unset(struct gpio_object *og)
{
    spin_lock(&og->pulse.lock);
    og->pulse.enable = 0;
    spin_unlock(&og->pulse.lock);
}


/**
 * @brief 向内核申请GPIO资源
 * 
 * @param og 
 * @return int 
 */
int extgpio_request_and_init(struct gpio_object *og)
{    
    int ret  = 0;
    struct gpio_info *info = &og->info;

    if (!info->do_not_request)
    {
        if ((ret = gpio_request(info->gpio.pin, info->name)) < 0) 
        {
            printk(KERN_INFO "Unabled to request gpio<%d> for '%s'\n", info->gpio.pin, info->name);
            return ret;
        }        
    }

    // 有可能失败，一些未申请的GPIO，还没有初始化
    if (info->gpio.dir_in)
    {
        if (gpio_direction_input(info->gpio.pin) < 0)
        {
            printk(KERN_INFO "Set input gpio<%d> failed\n", info->gpio.pin);
        }
    }
    else 
    {
        /* init output value */
        if (og->init_control || og->init_mask)
        {
            printk(KERN_INFO "Init %s with %08x %08x\n", info->name, og->init_control, og->init_mask);

            /* simple 1 !!! */
            if ((og->init_control == 1) && (og->init_mask == 0))
            {
                extgpio_set(&info->gpio, 1);
            }
            else 
            {
                if (og->init_mask == 0)
                {
                    og->init_mask = 0xffffffff;
                }

                extgpio_pulse_set(og, og->init_control, og->init_mask);
            }
        }
        else 
        {
            extgpio_set(&info->gpio, info->init_active);
        }
    }

    return 0;
}



/**
 * @brief 查找一个GPIO的对象
 * 
 * @param gdata 
 * @param name 
 * @return struct gpio_object* 
 */
struct gpio_object *extgpio_object_find(struct gpio_data *gdata, const char *name)
{
    struct gpio_object *p;

    list_for_each_entry(p, &gdata->gpios, list)
    {
        if (!strcmp(p->info.name, name))
        {
            return p;
        }
    }

    return NULL;
}


/**
 * @brief 创建一个GPIO的对象
 * 
 * @param gdata 
 * @param name 
 * @param pin 
 * @param active_low 
 * @param dir_in 
 * @return struct gpio_object* 
 */
struct gpio_object *extgpio_object_new(struct gpio_data * gdata, const char *name, int pin, int active_low, int dir_in)
{
    struct gpio_object *og;

    og = devm_kzalloc(gdata->dev, sizeof(*og), GFP_KERNEL);
    if (og == NULL)
    {
        printk(KERN_WARNING LOG_TAG "devm_kzalloc(%lu) failed\n", sizeof(*og));
        return NULL;
    }

    og->info.name = devm_kstrdup(gdata->dev, name, GFP_KERNEL);
    if (og->info.name == NULL)
    {
        printk(KERN_WARNING LOG_TAG "devm_kzalloc(%s) failed\n", name);
        devm_kfree(gdata->dev, og);        
        return NULL;
    }

    og->info.gpio.pin = pin;
    og->info.gpio.active_low = active_low;
    og->info.gpio.dir_in = dir_in;

    // 初始化脉冲控制器
    extgpio_pulse_init(&og->pulse);

    return og;
}


/**
 * @brief 删除一个GPIO的对象
 * 
 * @param gdata 
 * @param og 
 */
void extgpio_object_delete(struct gpio_data * gdata, struct gpio_object *og)
{
    devm_kfree(gdata->dev, (void *)og->info.name);
    devm_kfree(gdata->dev, og);
}





