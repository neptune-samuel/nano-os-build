
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

#include "gpio_type.h"
#include "lowlevel.h"

#ifdef CONFIG_OSMSG_NETLINK

#include <osmsg_api.h>

#else 

#define osmsg_button_event(_t, _p, _n)
#define osmsg_gpio_interrupt(_p, _n)

#endif // CONFIG_OSMSG_NETLINK

// button state
enum BUTTON_STATE
{
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_END
};

/// 默认的按键连击超时时间，如果该时间内再次出现按下事件，连击次数增加，否则，连击次数复位
#define BUTTON_DEFAULT_CONTINUE_CLICK_EXPIRED_MS   (HZ / 2)

/// 默认的进入长按状态的时间，可通过API修改
#define BUTTON_DEFAULT_LONG_PRESSED_SECOND       3
#define BUTTON_DEFAULT_LONG_PRESSED_TIME        (BUTTON_DEFAULT_LONG_PRESSED_SECOND * HZ)

/**
 * @brief 按键事件处理
 * 
 * @param obj 
 */
static void extgpio_button_irq_handle(struct gpio_object *obj)
{
    // 基本参数：
    // 按下后，进入长按的时间为 3秒
    // 连击超时时间，500ms, 按下后，不超过500ms后再次按下，视为连击
    
    int state = BUTTON_STATE_IDLE;
    unsigned long long_expired, click_expired;
    int click_count = 0;
    int long_pressed = 0;
    int next_state = state;

    while (state != BUTTON_STATE_END)
    {
        int pressed = extgpio_get(&obj->info.gpio);
        unsigned long now = jiffies;

        switch(state)
        {
            case BUTTON_STATE_IDLE:
            {

                if (pressed)
                {
                    //按健按下，去抖20ms
                    msleep(20);
                    pressed = extgpio_get(&obj->info.gpio);
                }

                if (pressed)
                {
                    //TODO: send event;
                    long_expired = jiffies + BUTTON_DEFAULT_LONG_PRESSED_TIME;
                    click_expired = jiffies + BUTTON_DEFAULT_CONTINUE_CLICK_EXPIRED_MS;

                    click_count = 1;
                    long_pressed = 0;

                    osmsg_button_event(OSMSG_BUTTON_EVENT_PRESSED, click_count, obj->info.name);

                    next_state = BUTTON_STATE_PRESSED;
                    printk("button[%s] pressed, click=%d\n", obj->info.name, click_count);
                }                
                else 
                {
                    // not a real click, exit
                    next_state = BUTTON_STATE_END;
                }
            }
            break;

            case BUTTON_STATE_PRESSED:
            {
                // 在按住状态下，如果GPIO一直按下，需要检测长按是否到达，长按到达后，设置长按状态
                if (pressed)
                {
                    if (!long_pressed && time_after(now, long_expired))
                    {
                        long_pressed = BUTTON_DEFAULT_LONG_PRESSED_SECOND;

                        click_count = 0;
                        /// 长按时，每隔一秒上报一个状态
                        long_expired = now + HZ;

                        // 发送按键消息
                        osmsg_button_event(OSMSG_BUTTON_EVENT_LONG_PRESSED, long_pressed, obj->info.name);
                        
                        printk("button[%s] long pressed, second=%d\n", obj->info.name, long_pressed);
                     
                    }
                    else if ((long_pressed > 0) && time_after(now, long_expired))
                    {
                        /// 长按时，每隔一秒上报一个状态
                        long_pressed ++;
                        long_expired = now + HZ; 
                        // 发送按键消息
                        osmsg_button_event(OSMSG_BUTTON_EVENT_LONG_PRESSED, long_pressed, obj->info.name);
                        
                        printk("button[%s] long pressed, second=%d\n", obj->info.name, long_pressed);               
                    }
                }

                // 如果释放了
                if (!pressed)
                {
                    // 去抖
                    msleep(20);
                    pressed = extgpio_get(&obj->info.gpio);

                    if (!pressed)
                    {
                        // 发送按键消息
                        osmsg_button_event(OSMSG_BUTTON_EVENT_RELEASED, click_count, obj->info.name);                       

                        //确认是释放                      
                        next_state = BUTTON_STATE_RELEASED;        

                        printk("button[%s] released, click=%d long pressed=%d\n", obj->info.name, click_count, long_pressed); 
                    }
                }
            }
            break;

            case BUTTON_STATE_RELEASED:
            {
                // click again
                if (pressed)
                {
                    // pressed again
                    msleep(20);
                    pressed = extgpio_get(&obj->info.gpio);
                }

                // click confirm
                if (pressed)
                {
                    next_state = BUTTON_STATE_PRESSED;
                    click_expired = jiffies + BUTTON_DEFAULT_CONTINUE_CLICK_EXPIRED_MS;                    

                    click_count ++;

                    // 发送按键消息
                    osmsg_button_event(OSMSG_BUTTON_EVENT_PRESSED, click_count, obj->info.name);

                    printk("button[%s] pressed, click=%d\n", obj->info.name, click_count);                     
                }
                else 
                {
                    // 在释放状态，
                    //   如果是长按释放，直接退出
                    //   如果是短按释放，时间未超过连击超时，继续等待到延时超时，
                    if (long_pressed)
                    {
                        next_state = BUTTON_STATE_END;
                    }
                    else if (click_count && time_after(now, click_expired))
                    {

                        // 发送按键消息
                        osmsg_button_event(OSMSG_BUTTON_EVENT_CONTINUE_CLICK_STOP, click_count, obj->info.name);                     
                        printk("button[%s] continue-click stop, click=%d\n", obj->info.name, click_count);    

                        // 复位连击次数
                        click_count = 0;

                        next_state = BUTTON_STATE_END;
                    }
                }
            }
            break;
        }

        if (state != next_state)
        {            
            state = next_state;
        }
        else 
        {
            // same state, delay few ms
            msleep(10);
        }
    }
}


/**
 * @brief 普通输入IO中断处理
 * 
 * @param obj 
 */
static void extgpio_input_irq_handle(struct gpio_object *obj)
{
    // send event to user space
    osmsg_gpio_interrupt(obj->info.gpio.pin, obj->info.name);

    printk("gpio[%s] interrupt\n", obj->info.name);
}


static irqreturn_t extgpio_irq_handle(int irq, void *handle)
{
    struct gpio_object *obj = handle;

    if (obj == NULL)
    {
        return IRQ_HANDLED;
    }

    // 先去除小的抖，后面可根据需求过滤，如，按键需要更大的延时
    msleep(10); /* for debounce */

    /* get reset status */
    if (!extgpio_get(&obj->info.gpio))
    {
        /* resume */
        return IRQ_HANDLED;
    }

    // use lock?
    if (obj->irq_in_handle)
    {
        /* previous interrupt in handle*/
        return IRQ_HANDLED;
    }

    obj->irq_in_handle = 1;

    if (obj->info.function == GPIO_FUNCTION_BUTTON)
    {
        extgpio_button_irq_handle(obj);
    }
    else 
    {
        extgpio_input_irq_handle(obj);
    }

    obj->irq_in_handle = 0;
    return IRQ_HANDLED;
}



int extgpio_with_interrupt( struct gpio_object *obj)
{
    // now, only button has interrupt 
    if (obj->info.function == GPIO_FUNCTION_BUTTON)
    {
        return 1;
    }

    return 0;    
}


int extgpio_set_interrupt(struct gpio_data *gdata, struct gpio_object *obj)
{
    int ret, irq;
    char name[128];
    unsigned long irq_flags;

    irq = gpio_to_irq(obj->info.gpio.pin);    
    if (irq < 0)
    {
        dev_err(gdata->dev, "Unable to get irq from %s\n", extgpio_pin_name(obj->info.gpio.pin, name));
        return -1;
    }
    
    irq_flags = IRQF_ONESHOT;
    
    if (obj->info.gpio.active_low)
    {
        /* 0 - power up , 1 - power down */
        irq_flags |= IRQF_TRIGGER_FALLING;
    }
    else
    {
        /* 1 - power up , 0 - power down */
        irq_flags |= IRQF_TRIGGER_RISING;
    }
    
    ret = devm_request_threaded_irq(gdata->dev, irq, NULL, extgpio_irq_handle, irq_flags, obj->info.name, obj);
    if (ret < 0) 
    {
        dev_err(gdata->dev, "Request irq(%d) for %s failed, ret = %d\n", irq, obj->info.name, ret);
        return -1;
    }

    obj->irq = irq;
    obj->with_interrupt = 1;
    obj->irq_in_handle = 0;
    dev_info(gdata->dev, "Request irq(%d) for %s success\n", irq, obj->info.name);

    return 0;
}

void extgpio_unset_interrupt(struct gpio_data *gdata, struct gpio_object *obj)
{
    if (obj->with_interrupt)
    {
        // TODO: free interrupt
        devm_free_irq(gdata->dev, obj->irq, obj);
        obj->with_interrupt = 0;
    }
}

