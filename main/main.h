#ifndef MAIN_H_
#define MAIN_H_

#include "freertos/event_groups.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
extern EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
#define CONNECTED_BIT  BIT0

#endif
