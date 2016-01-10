/*
 * frame.h
 *
 *  Created on: Dec 22, 2015
 *      Author: jnevens
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <stdint.h>

typedef struct frame_s frame_t;

typedef enum
{
	FRAME_PARSE_STATE_COMPLETE,
	FRAME_PARSE_STATE_INCOMPLETE,
	FRAME_PARSE_STATE_ERROR
} frame_parse_state_e;

frame_t *frame_create(uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status);
frame_t *frame_create_from_buffer(uint8_t *data, size_t len);
void frame_destroy(frame_t *frame);
size_t frame_get_raw_size(frame_t *frame);
size_t frame_get_size(frame_t *frame);
uint8_t *frame_get_data(frame_t *frame);
void frame_print(frame_t *frame);
frame_parse_state_e frame_parse(uint8_t *buf, size_t len);
uint8_t frame_calculate_crc(frame_t *frame);
int frame_send(frame_t *frame, int fd);

#endif /* FRAME_H_ */
