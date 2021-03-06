/*
 * frame.c
 *
 *  Created on: Dec 22, 2015
 *      Author: jnevens
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <libeltako/serial.h>
#include <libeltako/frame.h>

struct eltako_frame_s
{
	size_t len;
	uint8_t data[];
};

uint8_t eltako_frame_calculate_crc(eltako_frame_t *frame)
{
	uint8_t crc = 0;
	for(uint8_t i = 2; i < (frame->len - 1); i++)
		crc += frame->data[i];
	return crc;
}

eltako_frame_t *eltako_frame_create(uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status)
{
	eltako_frame_t *frame = (eltako_frame_t *)calloc(1, sizeof(eltako_frame_t) + 14);
	frame->len = 14;
	frame->data[0] = 0xA5;
	frame->data[1] = 0x5A;
	frame->data[2] = 0x0B;
	frame->data[3] = rorg;
	frame->data[4] = data[3];
	frame->data[5] = data[2];
	frame->data[6] = data[1];
	frame->data[7] = data[0];
	frame->data[8] = (uint8_t)((src >> 24) & 0xff);
	frame->data[9] = (uint8_t)((src >> 16) & 0xff);
	frame->data[10] = (uint8_t)((src >> 8) & 0xff);
	frame->data[11] = (uint8_t)((src) & 0x0ff);
	frame->data[12] = status;
	frame->data[13] = eltako_frame_calculate_crc(frame);
	return frame;
}

eltako_frame_t *eltako_frame_create_from_buffer(uint8_t *data, size_t len)
{
	eltako_frame_t *frame = (eltako_frame_t *)calloc(1, sizeof(eltako_frame_t) + len);
	memcpy(frame->data, data, len);
	frame->len = len;
	return frame;
}

void eltako_frame_destroy(eltako_frame_t *frame)
{
	free(frame);
}

size_t eltako_frame_get_raw_size(eltako_frame_t *frame)
{
	return frame->len;
}

size_t eltako_frame_get_size(eltako_frame_t *frame)
{
	return frame->data[2];
}

uint8_t *eltako_frame_get_data(eltako_frame_t *frame)
{
	return frame->data;
}

void eltako_frame_print(eltako_frame_t *frame)
{
	for (size_t i = 0; i < frame->len; i++) {
		printf("%02X ", frame->data[i]);
	}
	printf("\n");
}

eltako_frame_parse_state_e eltako_frame_parse(uint8_t *buf, size_t len)
{
	if (buf == NULL || len == 0)
		return FRAME_PARSE_STATE_ERROR;

	if (len >= 1 && buf[0] != 0xA5)
		return FRAME_PARSE_STATE_ERROR;

	if (len >= 2 && buf[1] != 0x5A)
		return FRAME_PARSE_STATE_ERROR;

	if (len < 3)
		return FRAME_PARSE_STATE_INCOMPLETE;

	uint8_t flen = buf[2];
	if ((flen + 3U) <= len) {
		uint8_t cs = 0;
		for(uint8_t i = 2; i < (flen + 2); i++)
			cs += buf[i];

		if(cs == buf[flen + 2]) {
			return FRAME_PARSE_STATE_COMPLETE;
		} else {
			printf("crc check failed!\n");
			return FRAME_PARSE_STATE_ERROR;
		}
	}

	return FRAME_PARSE_STATE_INCOMPLETE;
}

int eltako_frame_send(eltako_frame_t *frame, int fd)
{
	frame->data[frame->len - 1] = eltako_frame_calculate_crc(frame);

	return eltako_serial_write(fd, frame->data, frame->len);
}
