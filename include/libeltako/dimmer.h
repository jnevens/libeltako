/*
 * dimmer.h
 *
 *  Created on: Jan 2, 2016
 *      Author: jnevens
 */

#ifndef DIMMER_H_
#define DIMMER_H_

#include <stdbool.h>
#include <stdint.h>

#define DIMMER_EVENTS \
X(DIMMER_EVENT_ON, "on") \
X(DIMMER_EVENT_OFF, "off") \
X(DIMMER_EVENT_LRN, "learn")

#define X(a, b) a,
typedef enum {
	DIMMER_EVENTS
} dimmer_event_t;
#undef X

message_t *dimmer_create_message(uint32_t src, dimmer_event_t event, uint8_t dim_val, uint8_t dim_speed, bool lock);

#endif /* DIMMER_H_ */
