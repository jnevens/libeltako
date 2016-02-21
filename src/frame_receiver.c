/*
 * frame_receiver.c
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libeltako/frame_receiver.h>

struct eltako_frame_receiver_s
{
	uint8_t *data;
	size_t data_size;
};

eltako_frame_receiver_t *eltako_frame_receiver_init()
{
	return (eltako_frame_receiver_t *)calloc(1, sizeof(eltako_frame_receiver_t));
}

void eltako_frame_receiver_destroy(eltako_frame_receiver_t *receiver)
{
	eltako_frame_receiver_reset(receiver);
	free(receiver);
}

void eltako_frame_receiver_add_data(eltako_frame_receiver_t *receiver, uint8_t *data, size_t len)
{
	receiver->data = realloc(receiver->data, receiver->data_size + len);
	memcpy(&receiver->data[receiver->data_size], data, len);
	receiver->data_size += len;
}

void eltako_frame_receiver_reset(eltako_frame_receiver_t *receiver)
{
	free(receiver->data);
	receiver->data = NULL;
	receiver->data_size = 0;
}

void eltako_frame_receiver_trim_buffer(eltako_frame_receiver_t *receiver)
{
	if(receiver->data_size <= 1)
		return;

	// find next start byte
	size_t n = 0;
	for(n = 1; n < receiver->data_size; n++)
	{
		if(receiver->data[n] == 0xA5)
			break;
	}

	if(n == receiver->data_size && n > 1) {
		eltako_frame_receiver_reset(receiver);
	} else {
		printf("Buffer trim, shift left: %zu\n", n);
		uint8_t *new_data = malloc(receiver->data_size - n);
		memcpy(new_data, &receiver->data[n], receiver->data_size - n);
		free(receiver->data);
		receiver->data = new_data;
		receiver->data_size -= n;
	}
}

eltako_frame_t *eltako_frame_receiver_parse_buffer(eltako_frame_receiver_t *receiver)
{
	eltako_frame_t *frame = NULL;

	switch (eltako_frame_parse(receiver->data, receiver->data_size))
	{
	case FRAME_PARSE_STATE_COMPLETE:
	{
		// retrieve frame
		frame = eltako_frame_create_from_buffer(receiver->data, 3U + receiver->data[2]);

		eltako_frame_receiver_trim_buffer(receiver);
	}
		break;
	case FRAME_PARSE_STATE_ERROR:
		eltako_frame_receiver_trim_buffer(receiver);
		// frame_receiver_reset(receiver);
		break;
	default:
		break;
	}

	return frame;
}
