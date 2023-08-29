/**
 * @file pwm_beeper.c
 * @author Liu Chuansen (chuansen.liu-a1253@aqara.com/179712066@qq.com)
 * @brief PWM beeper
 * @version 0.1
 * @date 2022-01-18
 * 
 * @copyright Copyright (c) 2022
 * 
 * @note 
 *  实现PWM无源蜂鸣器，使用sys文件系统操作蜂鸣器
 *  echo "1000 20 0 20 2000 100" > beeper
 *  1000HZ play 20ms
 *  off 20ms
 *  2000HZ play 100ms
 * 
 * dts config
 * 
 * beeper {
 *  compatible: "pwm-beeper";
 *  pwm_id = <0>;
 *  //pwms = <&pwm 0 1000000 0>;    
 * }
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>


#define PWM_BEEPER_MAX_TONE_NUM   128

#define PWM_BEEPER_VERSION   "v1.0"

/**
 * @brief 定义一个声音元素，由频率和时间组成
 * 
 */
struct tone
{
    /// 声音频率， 单位为HZ
    uint32_t freq;
    /// 声音时间，单位ms
    uint32_t time;
};

/**
 * @brief driver private data
 * 
 */
struct pwm_beeper
{
    struct device *dev;
    struct pwm_device *pwm;
    int pwm_id;
    // 占空比，1-99
    int pwm_duty;
    int pwm_enable;

    int playing;

    /// 当前声音索引
    int tone_index;
    /// 总数
    int tone_num;
    /// 声音数据
    struct tone tones[PWM_BEEPER_MAX_TONE_NUM];

    /// 播放使用的定时器
    struct hrtimer timer;
};



/**
 * @brief 停止播放
 * 
 * @param bp 
 */
static void beeper_stop(struct pwm_beeper *bp)
{
    
    printk("beeper stop\n");

    // stop timer
    // if (hrtimer_active(&bp->timer))
    // {
    //     hrtimer_cancel(&bp->timer);
    // }
    pwm_disable(bp->pwm);

    bp->playing = 0;
}


/**
 * @brief 开始播放
 * 
 * @param bp 
 * @return int
 *  1 - playing
 *  0 - play done
 */
static int beeper_play(struct pwm_beeper *bp)
{
    int ret;

    // 如果时间参数为0，直接中止
    if ((bp->tone_index < bp->tone_num) && (bp->tones[bp->tone_index].time > 0))
    {
        // 配置PWM参数，根据当前频率(HZ)，计算时间(ns)
        // 2000 HZ -> 0.5ms   500 000 ns
        // 1000 HZ -> 1ms -> 1000 000 ns
        // 1      -> 1000ms -> 1000 000 000 ns 
        uint32_t period_ns = 0, duty_ns = 0;
        uint32_t ms, s, ns;
        if (bp->tones[bp->tone_index].freq > 0)
        {
            period_ns = (1000000000UL)/bp->tones[bp->tone_index].freq;
        }

        ms = bp->tones[bp->tone_index].time;
        s = ms / 1000;
        ns = (ms % 1000) * 1000 * 1000;

        // 如果为0，表示不发声,占空比为0
        if (period_ns == 0)
        {            
            pwm_disable(bp->pwm);
        }
        else 
        {
            duty_ns = (period_ns * bp->pwm_duty)/100;

            ret = pwm_config(bp->pwm, duty_ns, period_ns);
            if (ret)
            {
                dev_warn(bp->dev, "pwm_config(%d) failed, ret=%d\n", bp->pwm_id, ret);
            }

            ret = pwm_enable(bp->pwm);
            if (ret)
            {
                dev_warn(bp->dev, "pwm_enable(%d) failed, ret=%d\n", bp->pwm_id, ret);
            }  
        }
        
        // start timer
        printk("play tone[%d]: duty=%d(%d) period=%d s=%d, ns=%d\n", bp->tone_index,  duty_ns, bp->pwm_duty, period_ns, s, ns);

        if (bp->tone_index == 0)
        {
            hrtimer_start(&bp->timer, ktime_set(s, ns), HRTIMER_MODE_REL);     
        }
        else 
        {
            hrtimer_forward_now(&bp->timer, ktime_set(s, ns));
        }
        
        bp->playing = 1;
        bp->tone_index ++;

        return 1;
    }
    else 
    {
        beeper_stop(bp);
        return 0;
    }
}


static enum hrtimer_restart beeper_timer_handle(struct hrtimer *timer)
{
    struct pwm_beeper *bp = container_of(timer, struct pwm_beeper, timer);

    int ret = beeper_play(bp);
    return ret ? HRTIMER_RESTART : HRTIMER_NORESTART;
}


#define is_number(_x) (((_x) <= '9') && ((_x) >= '0'))


/**
 * @brief 显示当前的播放状态
 * 
 * @param dev 
 * @param attr 
 * @param buf 
 * @return ssize_t 
 */
static ssize_t pwm_beeper_get_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct pwm_beeper *beeper = dev_get_drvdata(dev);
    return sprintf(buf, "%d", beeper->playing);
}


/**
 * @brief 播放一首曲
 * 
 * @param dev 
 * @param attr 
 * @param buf 
 * @param count 
 * @return ssize_t 
 * 
 * @note 
 * "echo 1000 20 0 20 2000 20 > play"
 */
static ssize_t pwm_beeper_play(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    struct pwm_beeper *beeper = dev_get_drvdata(dev);

#define PHASE_FREQ  0
#define PHASE_TIME  1

    int i = 0, phase = PHASE_FREQ;
    char number[32];
    int offset = 0;
    uint32_t v;

    memset(number, 0, sizeof(number));

    // stop the beeper
    beeper_stop(beeper);

    beeper->tone_index = 0;
    beeper->tone_num = 0;

    while (offset < count)
    {
        if (is_number(buf[offset]))
        {            
            // 如果太长了，不设置
            if (i < sizeof(number) - 1)
            {
                number[i] = buf[offset];
                i ++;
            }     
        }
        else 
        { 
            if ((i > 0) && (beeper->tone_num < sizeof(beeper->tones)/sizeof(beeper->tones[0])))
            {
                v = simple_strtoul(number, NULL, 10);
                if (phase == PHASE_FREQ)
                {
                    beeper->tones[beeper->tone_num].freq = v;
                    phase = PHASE_TIME;
                }
                else 
                {                    
                    beeper->tones[beeper->tone_num].time = v;
                    phase = PHASE_FREQ;

                    //printk("tone[%d] freq=%d, time=%d\n", beeper->tone_num, beeper->tones[beeper->tone_num].freq, beeper->tones[beeper->tone_num].time );
                    beeper->tone_num ++;
                }
            }

            i  = 0;
            memset(number, 0, sizeof(number));
        }

        offset ++;
    }  

    if (beeper->tone_num > 0)
    {
        beeper_play(beeper);
    }

    return count;  
}

/**
 * @brief 显示占空比
 * 
 * @param dev 
 * @param attr 
 * @param buf 
 * @return ssize_t 
 */
static ssize_t pwm_beeper_get_duty(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct pwm_beeper *beeper = dev_get_drvdata(dev);
    return sprintf(buf, "%d", beeper->pwm_duty);
}



/**
 * @brief 设置占空比
 * 
 * @param dev 
 * @param attr 
 * @param buf 
 * @param count 
 * @return ssize_t 
 */
static ssize_t pwm_beeper_set_duty(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    struct pwm_beeper *beeper = dev_get_drvdata(dev);
    
    int i = 0, v;
    char input[32];
    int offset = 0;

    memset(input, 0, sizeof(input));

    while (offset < count)
    {
        if (is_number(buf[offset]))
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
                v = simple_strtoul(input, NULL, 10);

                if ((v >= 100) || (v <= 0))
                {
                    dev_err(beeper->dev, "Invalid duty value:%d\n", v);
                    return count;
                }

                beeper->pwm_duty = v;
                return count;

            }

            i  = 0;
            memset(input, 0, sizeof(input));            

        }

        offset ++;
    }  

    return count;  
}


/**
 * @brief GPIO SYSFS NODES
 * 
 */
static DEVICE_ATTR(status, S_IRUGO, pwm_beeper_get_status, NULL);
static DEVICE_ATTR(play, S_IWUSR, NULL, pwm_beeper_play);
static DEVICE_ATTR(duty, S_IRUGO | S_IWUSR, pwm_beeper_get_duty, pwm_beeper_set_duty);

static struct attribute *pwm_beeper_sysfs_attrs[] = {
    &dev_attr_status.attr,
    &dev_attr_play.attr,
    &dev_attr_duty.attr,
    NULL
};

static struct attribute_group pwm_beeper_attribute_group = {
    .attrs = pwm_beeper_sysfs_attrs 
};


/**
 * @brief probe a pwm beeper
 * 
 * @param pdev 
 * @return int 
 */
static int pwm_beeper_probe(struct platform_device *pdev)
{
    struct device_node *np;
    struct pwm_beeper *beeper;
    int ret, pwm_id = 0;

    np = pdev->dev.of_node;

    ret = of_property_read_u32(np, "pwm_id", &pwm_id);
    if (ret) 
    {
        dev_err(&pdev->dev, "Parsed PWM id from dts node failed, ret=%d\n", ret);
        return ret;
    }

    beeper = devm_kzalloc(&pdev->dev, sizeof(*beeper), GFP_KERNEL);
    if (beeper == NULL)
    {
        return -ENOMEM;
    }

    //data->dev = &pdev->dev;
    beeper->pwm = pwm_request(pwm_id, "beeper");
    if (IS_ERR(beeper->pwm))
    {
        ret = PTR_ERR(beeper->pwm);
        dev_err(&pdev->dev, "Failed to request pwm device(%d), ret=%d\n", pwm_id, ret);

        goto to_devm_kfree;
    }

    beeper->pwm_id = pwm_id;
    beeper->pwm_duty = 50;
    beeper->dev = &pdev->dev;

    dev_info(&pdev->dev, "PWM beeper on pwm-%d initilized success(%s)\n", beeper->pwm_id, PWM_BEEPER_VERSION);

    // first ,create sysfs entries 
    ret = sysfs_create_group(&pdev->dev.kobj, &pwm_beeper_attribute_group);
    if (ret)
    {
        dev_warn(&pdev->dev, "Create sysfs attribute group failed, ret=%d\n", ret);

        goto to_pwm_free;
    }
    
    // 初始化定时器
    hrtimer_init(&beeper->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    beeper->timer.function = beeper_timer_handle;

    dev_set_drvdata(&pdev->dev, beeper);

    return 0;

    // 错误处理
to_pwm_free:
    pwm_free(beeper->pwm);
    beeper->pwm = NULL;

to_devm_kfree:
    devm_kfree(&pdev->dev, beeper);

    return ret;
}


static int pwm_beeper_remove(struct platform_device *pdev)
{
    struct pwm_beeper *beeper = dev_get_drvdata(&pdev->dev);

    sysfs_remove_group(&beeper->dev->kobj, &pwm_beeper_attribute_group);

    // remove timer 
    if (hrtimer_active(&beeper->timer))
    {
        hrtimer_cancel(&beeper->timer);
    }

    if (beeper->pwm)
    {
        pwm_free(beeper->pwm);
        beeper->pwm = NULL;
    }

    /* free dev */
    devm_kfree(&pdev->dev, beeper);

    printk("pwm-beeper removed\n");

    return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id pwm_beeper_match[] = {
    { .compatible = "pwm-beeper"},
    { },
};

MODULE_DEVICE_TABLE(of, pwm_beeper_match);
#endif

static struct platform_driver pwm_beeper_driver = {
    .probe  = pwm_beeper_probe,
    .remove = pwm_beeper_remove,
    .driver = {
        .name    = "pwm-beeper",
        #ifdef CONFIG_OF
        .of_match_table = of_match_ptr(pwm_beeper_match),
        #endif
    },
};

module_platform_driver(pwm_beeper_driver);

MODULE_AUTHOR("Liu Chuansen <179712066@qq.com>");
MODULE_DESCRIPTION("PWM Beeper");
MODULE_LICENSE("GPL");
 
