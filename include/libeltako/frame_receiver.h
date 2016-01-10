/*
 * frame_receiver.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef FRAME_RECEIVER_H_
#define FRAME_RECEIVER_H_

#include "frame.h"

typedef struct frame_receiver_s frame_receiver_t;



frame_receiver_t *frame_receiver_init();
void frame_receiver_destroy(frame_receiver_t *receiver);
void frame_receiver_add_data(frame_receiver_t *receiver, uint8_t *data, size_t len);
void frame_receiver_reset(frame_receiver_t *receiver);
void frame_receiver_trim_buffer(frame_receiver_t *receiver);
frame_t *frame_receiver_parse_buffer(frame_receiver_t *receiver);

#endif /* FRAME_RECEIVER_H_ */
