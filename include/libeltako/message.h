/*
 * message.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "frame.h"

#define MSG_TYPES \
X(MSG_TYPE_BUTTON, "butten") \
X(MSG_TYPE_DIMMER, "dimmer")

#define X(a, b) a,
typedef enum {
	MSG_TYPES
} message_type_t;
#undef X

typedef struct message_s message_t;

message_t *message_create(message_type_t type, uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status);
message_t *message_create_from_frame(frame_t *frame);
void message_destroy(message_t *msg);
void message_print(message_t *msg);
frame_t *message_to_frame(message_t *msg);
int message_send(message_t *msg, int fd);

char *message_type_to_string(message_type_t type);

#endif /* MESSAGE_H_ */
