/*
 * button.h
 *
 *  Created on: Dec 25, 2015
 *      Author: jnevens
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "message.h"

#define BTN_EVENTS \
X(BTN_EVENT_PRESS, "press") \
X(BTN_EVENT_RELEASE, "release")

#define X(a, b) a,
typedef enum {
	BTN_EVENTS
} button_event_t;
#undef X

typedef enum {
	BTN_WIP_TOP_LEFT = 0x30,
	BTN_WIP_TOP_RIGHT = 0x70,
	BTN_WIP_BOTTOM_LEFT = 0x10,
	BTN_WIP_BOTTOM_RIGHT = 0x50,
} button_wip_t;

typedef struct button_s
{
	button_event_t event;
} button_t;

message_t *button_create_message(uint32_t src, button_wip_t btn, button_event_t event);

#endif /* BUTTON_H_ */
