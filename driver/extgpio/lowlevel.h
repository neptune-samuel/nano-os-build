

#ifndef __LOWLEVEL_H__
#define __LOWLEVEL_H__

#include <linux/string.h>
#include <linux/ctype.h>

#include "gpio_type.h"

#define DRIVER_NAME      "extgpio"

/**
 * @brief 返回gpio的名称
 * 
 * @param gpio 
 * @param buffer 
 * @return const char* 
 */
const char *extgpio_pin_name(int gpio, char *buffer);

/**
 * @brief 配置GPIO的状态
 * 
 * @param gpio 
 * @param value 
 */
void extgpio_set(gpio_t *gpio, int value);

/**
 * @brief 读取GPIO的值
 * 
 * @param gpio 
 * @return int 
 */
int extgpio_get(gpio_t *gpio);


/**
 * @brief GPIO初始化脉冲控制
 * 
 * @param pulse 
 */
void extgpio_pulse_init(struct gpio_pulse *pulse);

/**
 * @brief GPIO脉冲控制设置
 * 
 * @param og GPIO的对象
 * @param control 控制字
 * @param mask 控制掩码
 */
void extgpio_pulse_set(struct gpio_object *og, u32 control, u32 mask);

/**
 * @brief GPIO取消脉冲控制
 * 
 * @param og 
 */
void extgpio_pulse_unset(struct gpio_object *og);


/**
 * @brief 向内核申请GPIO资源
 * 
 * @param og 
 * @return int 
 */
int extgpio_request_and_init(struct gpio_object *og);

/**
 * @brief 查找一个GPIO的对象
 * 
 * @param gdata 
 * @param name 
 * @return struct gpio_object* 
 */
struct gpio_object *extgpio_object_find(struct gpio_data *gdata, const char *name);

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
struct gpio_object *extgpio_object_new(struct gpio_data * gdata, const char *name, int pin, int active_low, int dir_in);

/**
 * @brief 删除一个GPIO的对象
 * 
 * @param gdata 
 * @param og 
 */
void extgpio_object_delete(struct gpio_data * gdata, struct gpio_object *og);

#endif /* __LOWLEVEL_H__ */
