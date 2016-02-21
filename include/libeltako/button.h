/*
 * button.h
 *
 *  Created on: Dec 25, 2015
 *      Author: jnevens
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "message.h"

#define ELTAKO_BTN_EVENTS \
X(BTN_EVENT_PRESS, "press") \
X(BTN_EVENT_RELEASE, "release")

#define X(a, b) a,
typedef enum {
	ELTAKO_BTN_EVENTS
} eltako_button_event_t;
#undef X

typedef enum {
	BTN_WIP_TOP_LEFT = 0x30,
	BTN_WIP_TOP_RIGHT = 0x70,
	BTN_WIP_BOTTOM_LEFT = 0x10,
	BTN_WIP_BOTTOM_RIGHT = 0x50,
} eltako_button_wip_t;

typedef struct eltako_button_s
{
	eltako_button_event_t event;
} eltako_button_t;

eltako_message_t *eltako_button_create_message(uint32_t src, eltako_button_wip_t btn, eltako_button_event_t event);

#endif /* BUTTON_H_ */
