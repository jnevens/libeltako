/*
 * frame.h
 *
 *  Created on: Dec 22, 2015
 *      Author: jnevens
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <stdint.h>

typedef struct eltako_frame_s eltako_frame_t;

typedef enum
{
	FRAME_PARSE_STATE_COMPLETE,
	FRAME_PARSE_STATE_INCOMPLETE,
	FRAME_PARSE_STATE_ERROR
} eltako_frame_parse_state_e;

eltako_frame_t *eltako_frame_create(uint8_t rorg, uint8_t data[], uint32_t src, uint8_t status);
eltako_frame_t *eltako_frame_create_from_buffer(uint8_t *data, size_t len);
void eltako_frame_destroy(eltako_frame_t *frame);
size_t eltako_frame_get_raw_size(eltako_frame_t *frame);
size_t eltako_frame_get_size(eltako_frame_t *frame);
uint8_t *eltako_frame_get_data(eltako_frame_t *frame);
void eltako_frame_print(eltako_frame_t *frame);
eltako_frame_parse_state_e eltako_frame_parse(uint8_t *buf, size_t len);
uint8_t eltako_frame_calculate_crc(eltako_frame_t *frame);
int eltako_frame_send(eltako_frame_t *frame, int fd);

#endif /* FRAME_H_ */
