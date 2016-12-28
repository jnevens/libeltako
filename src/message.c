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
		ELTAKO_MSG_TYPES
};
#undef X

struct eltako_message_s
{
	eltako_message_type_t type;
	uint32_t src;
	uint8_t status;
	uint8_t rorg;
	uint8_t data[4];
};

eltako_message_t *eltako_message_create(eltako_message_type_t type, uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status)
{
	eltako_message_t *msg = calloc(1, sizeof(eltako_message_t));
	msg->type = type;
	msg->rorg = rorg;
	msg->src = src;
	msg->status = status;
	memcpy(msg->data, data, 4);

	return msg;
}

eltako_message_t *eltako_message_create_from_frame(eltako_frame_t *frame)
{
	eltako_message_t *msg = calloc(1, sizeof(eltako_message_t));
	msg->rorg = eltako_frame_get_data(frame)[3];
	msg->src = eltako_frame_get_data(frame)[8] << 24 | eltako_frame_get_data(frame)[9] << 16
			| eltako_frame_get_data(frame)[10] << 8 | eltako_frame_get_data(frame)[11];
	msg->status = eltako_frame_get_data(frame)[12];

	msg->data[3] = eltako_frame_get_data(frame)[4];
	msg->data[2] = eltako_frame_get_data(frame)[5];
	msg->data[1] = eltako_frame_get_data(frame)[6];
	msg->data[0] = eltako_frame_get_data(frame)[7];

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

void eltako_message_destroy(eltako_message_t *msg)
{
	free(msg);
}

void eltako_message_print(eltako_message_t *msg)
{
	printf("message: type:%s, rorg:%02x, data:%02x %02x %02x %02x, source 0x%08x, status: %02x\n",
			eltako_message_type_to_string(msg->type),
			msg->rorg,
			msg->data[3],
			msg->data[2],
			msg->data[1],
			msg->data[0],
			msg->src,
			msg->status);
}

int eltako_message_send(eltako_message_t *msg, int fd)
{
	eltako_frame_t *frame = eltako_message_to_frame(msg);
	int rv = eltako_frame_send(frame, fd);
	free(frame);
	return rv;
}

char *eltako_message_type_to_string(eltako_message_type_t type)
{
	return msg_types_name[type];
}

eltako_frame_t *eltako_message_to_frame(eltako_message_t *msg)
{
	return eltako_frame_create(msg->rorg, msg->data, msg->src, msg->status);
}

uint32_t eltako_message_get_address(eltako_message_t *msg)
{
	return msg->src;
}

uint8_t eltako_message_get_status(eltako_message_t *msg)
{
	return msg->status;
}

uint8_t eltako_message_get_rorg(eltako_message_t *msg)
{
	return msg->rorg;
}

const uint8_t *eltako_message_get_data(eltako_message_t *msg)
{
	return msg->data;
}

