/*
 * message.c
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libeltako/message.h>

#define X(a, b) b,
char *msg_types_name[] = {
		MSG_TYPES
};
#undef X

struct message_s
{
	message_type_t type;
	uint32_t src;
	uint8_t status;
	uint8_t rorg;
	uint8_t data[4];
};

message_t *message_create(message_type_t type, uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status)
{
	message_t *msg = calloc(1, sizeof(message_t));
	msg->type = type;
	msg->rorg = rorg;
	msg->src = src;
	msg->status = status;
	memcpy(msg->data, data, 4);

	return msg;
}

message_t *message_create_from_frame(frame_t *frame)
{
	message_t *msg = calloc(1, sizeof(message_t));
	msg->rorg = frame_get_data(frame)[3];
	msg->src = frame_get_data(frame)[8] << 24 | frame_get_data(frame)[9] << 16
			| frame_get_data(frame)[10] << 8 | frame_get_data(frame)[11];
	msg->status = frame_get_data(frame)[12];

	msg->data[3] = frame_get_data(frame)[4];
	msg->data[2] = frame_get_data(frame)[5];
	msg->data[1] = frame_get_data(frame)[6];
	msg->data[0] = frame_get_data(frame)[7];

	switch (msg->rorg)
	{
	case 0x05:
		msg->type = MSG_TYPE_BUTTON;
		break;
	case 0x07:
		msg->type = MSG_TYPE_DIMMER;
		break;
	default:
		free(msg);
		return NULL;
		break;
	}
	return msg;
}

void message_destroy(message_t *msg)
{
	free(msg);
}

void message_print(message_t *msg)
{
	printf("message: type:%s, rorg:%02x, data:%02x %02x %02x %02x, source 0x%08x, status: %02x\n",
			message_type_to_string(msg->type),
			msg->rorg,
			msg->data[3],
			msg->data[2],
			msg->data[1],
			msg->data[0],
			msg->src,
			msg->status);
}

int message_send(message_t *msg, int fd)
{
	frame_t *frame = message_to_frame(msg);
	int rv = frame_send(frame, fd);
	free(frame);
	return rv;
}

char *message_type_to_string(message_type_t type)
{
	return msg_types_name[type];
}

frame_t *message_to_frame(message_t *msg)
{
	return frame_create(msg->rorg, msg->data, msg->src, msg->status);
}
