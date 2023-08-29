

/**
 * @file osmsg.c
 * @author Liu Chuansen (179712066@qq.com)
 * @brief osmsg by netlink
 * @version 0.1
 * @date 2021-06-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
 
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/sockios.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>

#include <net/sock.h>
#include <net/netlink.h>

#include "osmsg_type.h"


static struct sock *osmsg_netlink_socket = NULL;

int osmsg_netlink_send(void *buf, uint16_t len)
{
    int ret;
    struct sk_buff *skb;
    struct nlmsghdr *nlh;

    skb = nlmsg_new(nlmsg_total_size(len), GFP_ATOMIC);

    if (skb == NULL)
    {
        printk("nlmsg_new() failed\n");
        return -1;
    }

    nlh = nlmsg_put(skb, 0, 0, NETLINK_OSMSG, len, 0);
    if (nlh == NULL)
    {
        printk("nlmsg_put() failed\n");    
        nlmsg_free(skb);
        return -1;
    }

    memcpy(nlmsg_data(nlh), buf, len);

    NETLINK_CB(skb).dst_group = 1;
    ret =  netlink_broadcast(osmsg_netlink_socket, skb, 0, 1, GFP_ATOMIC);

    return ret;
}

static void osmsg_netlink_receive(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;    
    int len, plen;

    nlh = nlmsg_hdr(skb);
    len = skb->len;
    
    while (NLMSG_OK(nlh, len)) 
    {        
        plen = NLMSG_PAYLOAD(nlh, 0);
        if (plen > 0)
        {
            // loop back message
            osmsg_netlink_send(NLMSG_DATA(nlh), plen);
        }

        /* if err or if this message says it wants a response */
       // if (nlh->nlmsg_flags & NLM_F_ACK)
       //     netlink_ack(skb, nlh, 0);
    
        nlh = NLMSG_NEXT(nlh, len);
    }
}


int osmsg_netlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = osmsg_netlink_receive,
        .groups = 1,
    };
    
    osmsg_netlink_socket = (struct sock *)netlink_kernel_create(&init_net, NETLINK_OSMSG, &cfg);
        
    if (!osmsg_netlink_socket) 
    {
        printk(KERN_ERR "unable to create osmsg netlink socket!\n");
        return -ENODEV;
    }

    printk(KERN_INFO "register osmsg netlink module\n");

    return 0;
}

void osmsg_netlink_exit(void)
{
    if (osmsg_netlink_socket)
    {
        sock_release(osmsg_netlink_socket->sk_socket);
    }

    printk(KERN_INFO "unregister osmsg netlink module\n");    
}

