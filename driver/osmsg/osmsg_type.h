
/**
 * @file osmsg_type.h
 * @author Liu Chuansen (179712066@qq.com)
 * @brief os message types
 * @version 0.1
 * @date 2021-06-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
 
#ifndef _OSMSG_TYPE_H_
#define _OSMSG_TYPE_H_


#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/ctype.h>
#else 
#include <stdint.h>
#endif 


/* netlink */
#ifndef CONFIG_OSMSG_NETLINK_TYPE  
#define CONFIG_OSMSG_NETLINK_TYPE  25
#endif 

#define NETLINK_OSMSG  CONFIG_OSMSG_NETLINK_TYPE

/* for netlink msg */
#define MAKE_OSMSG_TYPE(_d, _s, _t, _m) (((_d) << 24) | ((_s) << 16) | ((_t) << 8) | (_m))
#define OSMSG_DEST(_type) (((_type) >> 24) & 0xff)
#define OSMSG_SRC(_type) (((_type) >> 16) & 0xff)
#define OSMSG_TYPE(_type) (((_type) >> 8) & 0xff)
#define OSMSG_SUBTYPE(_type) ((_type) & 0xff)

typedef struct
{
   uint32_t type;
   uint32_t size;
}osmsg_header_t;

enum
{
    OSMSG_OS = 0,
    OSMSG_BROADCAST = 0xff
};


/* MSG TYPE */
enum
{
    OSMSG_TYPE_GPIO_INTERRUPT = 1,
    OSMSG_TYPE_BUTTON_EVENT = 2, 
    
    // 128 and upper are for user define
    OSMSG_TYPE_USER_BASE = 128,   
};


/**
 * @brief GPIO中断消息
 * 
 */
typedef struct 
{
    uint32_t gpio;
    char name[64];
}osmsg_gpio_interrupt_t;

/**
 * @brief 按键事件定义
 * 
 */
enum OSMSG_BUTTON_EVENT
{
    OSMSG_BUTTON_EVENT_PRESSED = 1,
    OSMSG_BUTTON_EVENT_RELEASED,
    OSMSG_BUTTON_EVENT_CONTINUE_CLICK_STOP,
    OSMSG_BUTTON_EVENT_LONG_PRESSED,
};

/**
 * @brief 消息内容
 * 
 */
typedef struct 
{
    uint32_t parameter;     
    char name[64];
}osmsg_button_event_t;

#endif /* _OSMSG_TYPE_H_  */

 
 