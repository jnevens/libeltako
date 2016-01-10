/*
 * dimmer.c
 *
 *  Created on: Jan 2, 2016
 *      Author: jnevens
 */
#include <string.h>

#include "message.h"
#include "dimmer.h"

message_t *dimmer_create_message(uint32_t src, dimmer_event_t event, uint8_t dim_val, uint8_t dim_speed, bool lock)
{
	uint8_t data[4] = { 0 };
	data[3] = 0x02;
	if(event != DIMMER_EVENT_LRN) {
		data[2] = dim_val;
		data[1] = dim_speed;
		data[0] = 0x08;
		data[0] |= (lock) ? 0x04 : 0x00;
		data[0] |= (event == DIMMER_EVENT_ON) ? 0x01 : 0x00;
	}
	return message_create(MSG_TYPE_DIMMER, 0x07, data, src, 0x00);
}
