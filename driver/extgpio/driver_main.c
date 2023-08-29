
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
 * Created by LiuChuansen 20210316
 */
 
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/uaccess.h>    // for copy_from_user  
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/sockios.h>
#include <linux/seq_file.h>   // for sequence files  
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/of_gpio.h>
#include <linux/version.h>

#include "gpio_type.h"
#include "lowlevel.h"
#include "sysfs.h"

#include "irq_handle.h"

/**
 * @brief 定义定时器的SLOT间隙
 * 
 */
#define GPIO_TIMER_PERIOD  (HZ/10)


/**
 * @brief GPIO的定时器
 * 
 * @param dummy 
 */
static void gpio_timer_handle( unsigned long dummy )
{
    int v;
    struct gpio_data *gdata = (struct gpio_data *)dummy;
    struct gpio_object *p;

    list_for_each_entry(p, &gdata->gpios, list)
    {
        if (p->pulse.enable)
        {
            spin_lock(&p->pulse.lock);

            /* mask == 0 treat as full mask */
            v = p->pulse.control & (1 << p->pulse.offset);

            if (p->pulse.mask && !(p->pulse.mask & (1 << p->pulse.offset)))
            {
                /* no looping, set disable */
                p->pulse.enable = 0;
            }
            else 
            {
                /* set gpio */
                extgpio_set(&p->info.gpio, v);
            }

            if (p->pulse.offset >= 31)
            {
                p->pulse.offset = 0;
            }
            else
            {
                p->pulse.offset ++;
            }

            spin_unlock(&p->pulse.lock);
        }
    }

    mod_timer(&gdata->gpio_timer, jiffies + GPIO_TIMER_PERIOD);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)

static void gpio_timer_handle_new(struct timer_list *t)
{
    struct gpio_data *gdata = from_timer(gdata, t, gpio_timer);
    gpio_timer_handle((unsigned long)gdata);   
}

#endif 

/**
 * @brief 解析DTS，并生成GPIO结节，必须已初始化了相应的SYSFS NODES
 * 
 * @param gdata 
 * @return int 
 */
static int import_gpio_from_dts(struct gpio_data *gdata)
{
    int gpio, ret;
    u32 value;
    char buffer[32];// for gpio name
    struct device_node *child_node;
    struct device_node *root;
    u32 array[2];
    struct gpio_object *og;
    int private_gpio_dt = 0;

    root = of_get_child_by_name(gdata->dev->of_node, "gpios");
    if (!root) 
    {
        dev_warn(gdata->dev, "Could not find 'gpios' node\n");
        return 0;
    }

    if (!of_device_is_available(root))
    {
        dev_warn(gdata->dev, "Node 'gpios' is disabled\n");
        return 0;
    }
    
    /* parse each gpio node */
    for_each_child_of_node(root, child_node) 
    {
        int active_low = 0;

        if (!of_device_is_available(child_node))
        {
            continue;
        }

        /* get gpio */
        // linux type of gpio defines?
        if (!private_gpio_dt)
        {
            gpio = of_get_named_gpio_flags(child_node, "gpio", 0, &value);
            if (gpio < 0)
            {
                ret = gpio;
                dev_warn(gdata->dev, "Parse gpio node '%s' failed, ret = %d\n", child_node->name, ret);
                // try private type of gpio defines
                private_gpio_dt = 1;
            }
            else 
            {
                active_low = (value & OF_GPIO_ACTIVE_LOW) ? 1 : 0;
            }
        }

        if (private_gpio_dt)
        {
            ret = of_property_read_u32(child_node, "gpio", &value);
            if (ret) 
            {
                dev_warn(gdata->dev, "Parse gpio node '%s' failed, ret = %d\n", child_node->name, ret);
                continue;
            }

            gpio = value;
            
            active_low = of_property_read_bool(child_node, "active-low");
        }

        // 创建一个GPIO的对象
        og = extgpio_object_new(gdata, child_node->name, gpio, active_low, 0);
        if (og == NULL)
        {
            continue;
        }

        og->info.gpio.dir_in  = of_property_read_bool(child_node, "direction-in");
        og->info.init_active = of_property_read_bool(child_node, "active-on-linux");
        og->info.do_not_request = of_property_read_bool(child_node, "not-request");

        ret = of_property_read_u32(child_node, "function", &value);
        if (!ret) 
        {
            og->info.function = value;
        }

        if ((og->info.function == GPIO_FUNCTION_LED)
            || (og->info.function == GPIO_FUNCTION_BUZZER))
        {
            og->info.gpio.dir_in = 0;
        }
        else if (og->info.function == GPIO_FUNCTION_BUTTON)
        {
            og->info.gpio.dir_in = 1;
        }

        /* get init value */
        if (of_property_read_u32_array(child_node, "init", array, 2) == 0)
        {
            og->init_control = array[0];
            og->init_mask = array[1];
        }

        dev_info(gdata->dev, "Add gpio '%s' with pin %d(%s) (active:%d,in:%d,init:%d,request:%d)\n", og->info.name, og->info.gpio.pin, 
            extgpio_pin_name(og->info.gpio.pin, buffer), og->info.gpio.active_low,
            og->info.gpio.dir_in, og->info.init_active, !og->info.do_not_request);

        list_add_tail(&og->list, &gdata->gpios);
    }

    return 1;
}


/**
 * @brief 初始化已登记的GPIO
 * 
 * @param gdata 
 * @return int 
 */
static int extgpio_init_all(struct gpio_data *gdata)
{
    int num = 0;
    struct gpio_object *p;
    
    list_for_each_entry(p, &gdata->gpios, list)
    {
        extgpio_request_and_init(p);

        if (extgpio_with_interrupt(p))
        {
            extgpio_set_interrupt(gdata, p);
        }

        // 添加一个结点
        sysfs_add_gpio_node(gdata, p);

        num ++;
    }

    dev_info(gdata->dev, "Init %d gpios\n", num);

    return num;
}

/**
 * @brief 释放所有登记的GPIO资源
 * 
 * @param gdata 
 */
static void extgpio_free_all(struct gpio_data *gdata)
{
    struct gpio_object *p, *tmp;

    list_for_each_entry_safe(p, tmp, &gdata->gpios, list)
    {
        // unset interrupt
        extgpio_unset_interrupt(gdata, p);

        /* remove sysfs */
        sysfs_remove_file(gdata->gpio_kobj, &p->kobj_attr.attr);

        list_del(&p->list);
        
        if (!p->info.do_not_request)
        {
            gpio_free(p->info.gpio.pin);
        }

        dev_info(gdata->dev, "Remove gpio '%s'", p->info.name);

        extgpio_object_delete(gdata, p);
    }
}

/**
 * @brief GPIO SYSFS NODES
 * 
 */
static DEVICE_ATTR(gpio_info, S_IRUGO, show_gpio_info, NULL);
static DEVICE_ATTR(export, S_IWUSR, NULL, sysfs_add_gpio);
static DEVICE_ATTR(unexport, S_IWUSR, NULL, sysfs_remove_gpio);

static struct attribute *extgpio_sysfs_attrs[] = {
    &dev_attr_gpio_info.attr,
    &dev_attr_export.attr,
    &dev_attr_unexport.attr,
    NULL
};

static struct attribute_group extgpio_attribute_group = {
    .attrs = extgpio_sysfs_attrs 
};


extern int extgpio_init_buttons(struct gpio_data *gdata);

/**
 * @brief 驱动总入口，初始化
 * 
 * @param gdata 管理结构体
 * @return int 
 */

int extgpio_init(struct gpio_data *gdata)
{
    int ret;
    int gpio_num;

    INIT_LIST_HEAD(&gdata->gpios);

    // first ,create sysfs entries 
    ret = sysfs_create_group(&gdata->dev->kobj, &extgpio_attribute_group);
    if (ret)
    {
        dev_warn(gdata->dev, "Create extgpio attribute group failed, ret=%d\n", ret);
        return ret;
    }

    // create sysfs nodes for gpios
    gdata->gpio_kobj = kobject_create_and_add("gpios", &gdata->dev->kobj);

    // find gpios from dts 
    import_gpio_from_dts(gdata);

    // 初始化已登记的GPIO
    gpio_num = extgpio_init_all(gdata);

    // 起用并设置GPIO定时器
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    setup_timer(&gdata->gpio_timer, gpio_timer_handle, (unsigned long)gdata);
    #else  
    timer_setup(&gdata->gpio_timer, gpio_timer_handle_new, 0);
    #endif 

    mod_timer(&gdata->gpio_timer, jiffies + GPIO_TIMER_PERIOD);

    // 打印信息
    dev_info(gdata->dev, "Load %d gpios\n", gpio_num);     

    return 0;
}

/**
 * @brief 驱动总入口，释放资源
 * 
 * @param gdata 
 */
void extgpio_exit(struct gpio_data *gdata)
{
    // 删除定时器
    del_timer(&gdata->gpio_timer);

    // 释放所有GPIO的对象
    extgpio_free_all(gdata);

    // 释放gpio的KOBJ
    if (gdata->gpio_kobj)
    {
        kobject_del(gdata->gpio_kobj);
    }

    // 删除设备的KOBJ
    sysfs_remove_group(&gdata->dev->kobj, &extgpio_attribute_group);
}


/**
 * @brief 驱动入口，探测函数
 * 
 * @param pdev 
 * @return int 
 */
static int extgpio_probe(struct platform_device *pdev)
{
    struct gpio_data *data;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);

    if (data == NULL)
    {
        return -ENOMEM;
    }

    data->dev = &pdev->dev;

    /* set drv data */
    dev_set_drvdata(&pdev->dev, data);

    /* init gpio */
    extgpio_init(data);

    return 0;
}

static int extgpio_remove(struct platform_device *pdev)
{
    struct gpio_data *data = dev_get_drvdata(&pdev->dev);

    extgpio_exit(data);

    /* free dev */
    devm_kfree(&pdev->dev, data);

    return 0;
}

#ifdef CONFIG_EXTGPIO_USE_DTS

static const struct of_device_id extgpio_match_ids[] = {
    { .compatible = "extgpio-generic", .data = NULL},
    {},
};

MODULE_DEVICE_TABLE(of, extgpio_match_ids);
#endif 

static struct platform_driver extgpio_driver = {
.probe          = extgpio_probe,
.remove         = extgpio_remove,
.driver         = {
    .name               = DRIVER_NAME,
    .owner              = THIS_MODULE,
#ifdef CONFIG_EXTGPIO_USE_DTS    
    .of_match_table = extgpio_match_ids,
#endif     
    },
};

#ifdef CONFIG_EXTGPIO_USE_DTS 
module_platform_driver(extgpio_driver);

#else 

static struct platform_device *extgpio_device;


static int extgpio_driver_init(void)
{
    platform_driver_register(&extgpio_driver);

    extgpio_device = platform_device_register_simple(DRIVER_NAME, -1, NULL, 0);

    return 0;
}

static void extgpio_driver_exit(void)
{
    platform_device_unregister(extgpio_device);
    platform_driver_unregister(&extgpio_driver);
}

module_init(extgpio_driver_init);
module_exit(extgpio_driver_exit);

#endif 

MODULE_AUTHOR("Liu Chuansen <179712066@qq.com>");
MODULE_DESCRIPTION("General EXT-GPIO Framework Driver");
MODULE_LICENSE("GPL");

