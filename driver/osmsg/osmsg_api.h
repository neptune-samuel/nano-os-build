
#ifndef __OSMSG_API_H__
#define __OSMSG_API_H__

#include <linux/string.h>
#include <linux/ctype.h>

#include "osmsg_type.h"

/**
 * @brief 发送按键事件
 * 
 * @param type 事件类型，
 * @param parameter 事件参数
 * @param name 按键名称
 * @return int 
 */
int osmsg_button_event(uint8_t type, uint32_t parameter, const char *name);

/**
 * @brief 发送GPIO中断事件
 * 
 * @param gpio GPIO索引 
 * @param name GPIO名称
 * @return int 
 */
int osmsg_gpio_interrupt(uint32_t gpio, const char *name);

#endif // __OSMSG_API_H__
