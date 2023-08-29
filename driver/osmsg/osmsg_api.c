
#include <linux/module.h>

#include "osmsg_type.h"
#include "osmsg_netlink.h"


int osmsg_button_event(uint8_t type, uint32_t parameter, const char *name)
{
    char buffer[sizeof(osmsg_header_t) + sizeof(osmsg_button_event_t)];
    osmsg_header_t *msg = (osmsg_header_t *)buffer;
    osmsg_button_event_t *event = (osmsg_button_event_t *)(msg + 1);

    memset(buffer, 0, sizeof(buffer));

    /* set type */
    msg->type = MAKE_OSMSG_TYPE(OSMSG_BROADCAST, OSMSG_OS, OSMSG_TYPE_BUTTON_EVENT, type);
    msg->size = sizeof(*event);

    event->parameter = parameter;
    if (name && name[0])
    {
        strncpy(event->name, name, sizeof(event->name)); 
    }
    
    return osmsg_netlink_send(msg, sizeof(buffer));
}


int osmsg_gpio_interrupt(uint32_t gpio, const char *name)
{
    char buffer[sizeof(osmsg_header_t) + sizeof(osmsg_gpio_interrupt_t)];
    osmsg_header_t *msg = (osmsg_header_t *)buffer;
    osmsg_gpio_interrupt_t *event = (osmsg_gpio_interrupt_t *)(msg + 1);

    memset(buffer, 0, sizeof(buffer));

    /* set type */
    msg->type = MAKE_OSMSG_TYPE(OSMSG_BROADCAST, OSMSG_OS, OSMSG_TYPE_GPIO_INTERRUPT, 0);
    msg->size = sizeof(*event);

    event->gpio = gpio;

    if (name && name[0])
    {
        strncpy(event->name, name, sizeof(event->name)); 
    } 

    return osmsg_netlink_send(msg, sizeof(buffer));
}

EXPORT_SYMBOL(osmsg_gpio_interrupt);
EXPORT_SYMBOL(osmsg_button_event);
