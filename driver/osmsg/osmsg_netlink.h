

#ifndef _OSMSG_NETLINK_H_
#define _OSMSG_NETLINK_H_

#include "osmsg_type.h"

int osmsg_netlink_send(void *buf, uint16_t len);

int osmsg_netlink_init(void);

void osmsg_netlink_exit(void);

#endif // _OSMSG_NETLINK_H_
