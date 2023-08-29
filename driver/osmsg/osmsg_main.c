

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

#include "osmsg_netlink.h"

static int osmsg_init(void)
{

    osmsg_netlink_init();

    return 0;
}

static void osmsg_exit(void)
{
    osmsg_netlink_exit();
}

module_init(osmsg_init);
module_exit(osmsg_exit);

MODULE_AUTHOR("Liu Chuansen <179712066@qq.com>");
MODULE_DESCRIPTION("osmsg module for user space application");
MODULE_LICENSE("GPL");

