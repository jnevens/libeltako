/*
 * button.c
 *
 *  Created on: Jan 2, 2016
 *      Author: jnevens
 */

#include <string.h>

#include <libeltako/button.h>

eltako_message_t *eltako_button_create_message(uint32_t src, eltako_button_wip_t btn, eltako_button_event_t event)
{
	uint8_t data[4] = { 0 };
	data[3] = (event == BTN_EVENT_PRESS) ? btn : 0;
	return eltako_message_create(MSG_TYPE_BUTTON, 0x05, data, src, (event == BTN_EVENT_PRESS) ? 0x30 : 0x20);
}
