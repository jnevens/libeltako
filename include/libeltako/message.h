/*
 * message.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "frame.h"

#define ELTAKO_MSG_TYPES \
X(MSG_TYPE_BUTTON, "butten") \
X(MSG_TYPE_DIMMER, "dimmer") \
X(MSG_TYPE_BLIND, "blind")

#define X(a, b) a,
typedef enum {
	ELTAKO_MSG_TYPES
} eltako_message_type_t;
#undef X

typedef struct eltako_message_s eltako_message_t;

eltako_message_t *eltako_message_create(eltako_message_type_t type, uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status);
eltako_message_t *eltako_message_create_from_frame(eltako_frame_t *frame);
void eltako_message_destroy(eltako_message_t *msg);
void eltako_message_print(eltako_message_t *msg);
eltako_frame_t *eltako_message_to_frame(eltako_message_t *msg);
int eltako_message_send(eltako_message_t *msg, int fd);

uint32_t eltako_message_get_address(eltako_message_t *msg);
uint8_t eltako_message_get_status(eltako_message_t *msg);
uint8_t eltako_message_get_rorg(eltako_message_t *msg);
const uint8_t *eltako_message_get_data(eltako_message_t *msg);

char *eltako_message_type_to_string(eltako_message_type_t type);

#endif /* MESSAGE_H_ */
