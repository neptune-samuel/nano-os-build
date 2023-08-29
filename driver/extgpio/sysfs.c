
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/sockios.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/sysfs.h>


#include "lowlevel.h"
#include "irq_handle.h"

ssize_t sysfs_set_gpio(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count)
{
    struct device *dev = container_of(kobj->parent, struct device, kobj);
    struct gpio_data *gdata = dev_get_drvdata(dev);
    struct gpio_object *gpio;
    u32 control, mask;
    char *endp;
	size_t len = count;

    gpio = extgpio_object_find(gdata, attr->attr.name);
    if (gpio == NULL)
    {
        printk(KERN_WARNING LOG_TAG "gpio '%s' empty!!!\n", attr->attr.name);
        return 0;
    }  

	/* trim trailing whitespace */
	while (len && isspace(buf[len - 1]))
    {
        --len;
    } 

    if ((len == 2) && !strncasecmp(buf, "in", 2))
    {
        gpio->info.gpio.dir_in = 1;
        gpio_direction_input(gpio->info.gpio.pin);

        printk(KERN_INFO LOG_TAG "gpio '%s' set as input\n", attr->attr.name);
        return count;
    }
    else if ((len == 3) && !strncasecmp(buf, "out", 3))
    {
        gpio->info.gpio.dir_in = 0;
        // gpio set will auto set it as output gpio
        // gpio_direction_output(gpio->pin, gpio->active_low ? !value : value); 
        printk(KERN_INFO LOG_TAG "gpio '%s' set as output\n", attr->attr.name);        
        return count;
    }
    else if ((len == 6) && !strncasecmp(buf, "irq_on", 6))
    {
        if (gpio->with_interrupt)
        {
            printk(KERN_INFO LOG_TAG "gpio '%s' interrupt is already on\n", attr->attr.name);    
            return count;            
        }

        extgpio_set_interrupt(gdata, gpio);
        printk(KERN_INFO LOG_TAG "gpio '%s' interrupt on\n", attr->attr.name);        
        return count;
    }
    else if ((len == 7) && !strncasecmp(buf, "irq_off", 7))
    {
        extgpio_unset_interrupt(gdata, gpio);
        printk(KERN_INFO LOG_TAG "gpio '%s' interrupt off\n", attr->attr.name);         
        return count;
    }
    
    /*
        echo 0 > gpio 
        echo 1 > gpio 

        echo 0x0f0f0f0f 0xfffffff > gpio
        echo 0x0f0f0f0f 0x1f > gpio
    */
    control = simple_strtoul(buf, &endp, 16);
    mask = 0;

    /* check if two values */
    if (*endp && isxdigit(*(endp + 1)))
    {
        mask = simple_strtoul(endp + 1, NULL, 16);

        /* if 0 0 provide, turn pulse off */
        if ((control == 0) && (mask == 0))
        {
            extgpio_pulse_unset(gpio);
            extgpio_set(&gpio->info.gpio, 0);
        }
        else
        {
            extgpio_pulse_set(gpio, control, mask);
        }
       
    }
    else
    {
        /* only one value ? */
        if (control == 0 || control == 1)
        {
            extgpio_pulse_unset(gpio);
            extgpio_set(&gpio->info.gpio, control);
        }
        else 
        {
            /* enable control with mask 0 */
            extgpio_pulse_set(gpio, control, 0);
        }
    }

    return count;
}

ssize_t sysfs_get_gpio(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct device *dev = container_of(kobj->parent, struct device, kobj);
    struct gpio_data *gdata = dev_get_drvdata(dev);
    struct gpio_object *gpio;
    int value = -1;

    gpio = extgpio_object_find(gdata, attr->attr.name);

    if (gpio)
    {
        value = extgpio_get(&gpio->info.gpio);
    }

    return sprintf(buf, "%d\n", value);
}


ssize_t show_gpio_info(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct gpio_object *p;
    int len = 0;
    char buffer[32];
    struct gpio_data *gdata = dev_get_drvdata(dev);

    if (gdata == NULL)
    {
        printk(KERN_WARNING LOG_TAG "gpio data empty!!!\n");
        return 0;
    }

    len = sprintf(buf, "Name              GPIO          Direction   Active \n");

    list_for_each_entry(p, &gdata->gpios, list)
    {
        len += sprintf(buf + len, "%-16s  %s/%-3d  %-8s    %s\n", p->info.name, 
            extgpio_pin_name(p->info.gpio.pin, buffer), p->info.gpio.pin, 
            p->info.gpio.dir_in ? "input" : "output",
            p->info.gpio.active_low ? "low" : "high");
    }

    return len;
}


int sysfs_add_gpio_node(struct gpio_data *gdata, struct gpio_object *gpio)
{
    int ret = 0;

    /* set sysfs attr */
    gpio->kobj_attr.attr.name = gpio->info.name;
    gpio->kobj_attr.attr.mode = S_IWUSR | S_IRUGO; // 可读可写的
    gpio->kobj_attr.show = sysfs_get_gpio;
    gpio->kobj_attr.store = sysfs_set_gpio;

    ret = sysfs_create_file(gdata->gpio_kobj, &gpio->kobj_attr.attr);
    if (ret)
    {
        printk(KERN_WARNING LOG_TAG "Create gpio node '%s' failed, ret=%d\n", gpio->info.name, ret);
    }    

    return ret;
}


#define valid_char(_x) ((((_x) <= 'z') && ((_x) >= 'a')) || (((_x) <= 'Z') && ((_x) >= 'A')) || (((_x) <= '9') && ((_x) >= '0')) || ((_x) == '_') || ((_x) == '-'))

/**
 * @brief 动态添加一个GPIO结点
 * 
 * @param kobj 
 * @param attr 
 * @param buf 
 * @param count 
 * @return ssize_t 
 * 
 * @details 
 * echo name 3 [out active-low not-request led button] > gpio_add
 */

ssize_t sysfs_add_gpio(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    struct gpio_data *gdata = dev_get_drvdata(dev);
    struct gpio_object *gpio = NULL;

    char gpioName[32];

    // 查找输入变量，第一个固定为 gpio的名称，第二个固定为 GPIO引脚
    int varIndex = 0;
    int i = 0;
    char input[32];
    int offset = 0;

    while (offset < count)
    {
        if (valid_char(buf[offset]))
        {            
            // 如果太长了，不设置
            if (i < sizeof(input) - 1)
            {
                input[i] = buf[offset];
                i ++;
            }     
        }
        else 
        { 
            // 一个变量的终结，如果为i = -1, 变量为空，如开始字符是空格，或者连续几个空格
            if (i > 0)
            {
                input[i] = '\0';

                if (varIndex == 0)
                {
                    // 查找这个名字是否已有了，然后设置到gpioName中
                    strcpy(gpioName, input);

                    gpio = extgpio_object_find(gdata, gpioName);
                    if (gpio)
                    {
                        printk(KERN_WARNING LOG_TAG "gpio '%s' already exist!!!\n", gpioName);
                        return count;
                    }
                }
                else if (varIndex == 1)
                {
                    // 确定gpio index是否可用
                    int pin = simple_strtoul(input, NULL, 10);

                    if (!gpio_is_valid(pin))
                    {
                        printk(KERN_WARNING LOG_TAG "invalid gpio pin '%d'!!!\n", pin);
                        return count;                        
                    }

                    gpio = extgpio_object_new(gdata, gpioName, pin, 0, 1); // default is high active input pin                    
                    if (gpio == NULL)
                    {
                        printk(KERN_WARNING LOG_TAG "create gpio<%d> object failed!!!\n", pin);
                        return count;                          
                    }
                }
                else 
                {
                    // 动态设置GPIO的属性
                    // in out active-low not-request led button
                    if (!strcmp(input, "out"))
                    {
                        gpio->info.gpio.dir_in = 0;
                    }
                    else if (!strcmp(input, "in"))
                    {
                        gpio->info.gpio.dir_in = 1;
                    }
                    else if (!strcmp(input, "active-low"))
                    {
                        gpio->info.gpio.active_low = 1;
                    }
                    else if (!strcmp(input, "not-request"))
                    {
                        gpio->info.do_not_request = 1;
                    }    
                    else if (!strcmp(input, "led"))
                    {
                        gpio->info.function = GPIO_FUNCTION_LED;
                    }       
                    else if (!strcmp(input, "button"))
                    {
                        gpio->info.function = GPIO_FUNCTION_BUTTON;
                    } 
                    else 
                    {
                        printk(KERN_WARNING LOG_TAG "unknown option '%s'!!!\n", input);
                        printk(KERN_WARNING LOG_TAG "supported options: in out active-low not-request led button\n");
                    } 
                }
                
                varIndex ++;
            }

            i = 0;            
        }

        offset ++;
    }

    if (gpio == NULL)
    {
        printk(KERN_WARNING LOG_TAG ":unknown gpio line\n");
        return count;                          
    }

    printk(KERN_INFO LOG_TAG "Add gpio '%s' with pin %d(%s) (active:%d,in:%d,init:%d,request:%d)\n", gpio->info.name, gpio->info.gpio.pin, 
        extgpio_pin_name(gpio->info.gpio.pin, gpioName), !gpio->info.gpio.active_low,
        gpio->info.gpio.dir_in, gpio->info.init_active, !gpio->info.do_not_request);

    // 添加到队列中
    list_add_tail(&gpio->list, &gdata->gpios);

    // 请求并初始化
    extgpio_request_and_init(gpio);

    // 添加一个结点
    sysfs_add_gpio_node(gdata, gpio);

    return count;
}


/**
 * @brief 删除一个GPIO
 * 
 * @param kobj 
 * @param attr 
 * @param buf 
 * @param count 
 * @return ssize_t 
 */
ssize_t sysfs_remove_gpio(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    struct gpio_data *gdata = dev_get_drvdata(dev);
    struct gpio_object *gpio;

    int i = 0;
    char input[32];
    int offset = 0;

    while (offset < count)
    {
        if (valid_char(buf[offset]))
        {            
            // 如果太长了，不设置
            if (i < sizeof(input) - 1)
            {
                input[i] = buf[offset];
                i ++;
            }     
        }
        else 
        { 
            if (i > 0)
            {
                // 找到第一个变量
                input[i] = '\0';

                gpio = extgpio_object_find(gdata, input);
                if (gpio == NULL)
                {
                    printk(KERN_WARNING LOG_TAG "gpio '%s' does not exist!!!\n", input);
                    return count;                          
                }

                // 删除sysfs的结点
                sysfs_remove_file(gdata->gpio_kobj, &gpio->kobj_attr.attr);

                // 删除这个结点
                list_del(&gpio->list);
                
                // 释放GPIO资源 
                if (!gpio->info.do_not_request)
                {
                    gpio_free(gpio->info.gpio.pin);
                }

                printk(KERN_INFO LOG_TAG "Remove gpio '%s'", gpio->info.name);
                // 删除对象
                extgpio_object_delete(gdata, gpio);

                return count;

            }

            i  = 0;
        }

        offset ++;
    }  

    return count;  
}