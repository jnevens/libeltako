/*
 * button.c
 *
 *  Created on: Jan 2, 2016
 *      Author: jnevens
 */

#include <string.h>

#include "button.h"

message_t *button_create_message(uint32_t src, button_wip_t btn, button_event_t event)
{
	uint8_t data[4] = { 0 };
	data[3] = (event == BTN_EVENT_PRESS) ? btn : 0;
	return message_create(MSG_TYPE_BUTTON, 0x05, data, src, (event == BTN_EVENT_PRESS) ? 0x30 : 0x20);
}
