/*
 * frame_receiver.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef FRAME_RECEIVER_H_
#define FRAME_RECEIVER_H_

#include "frame.h"

typedef struct eltako_frame_receiver_s eltako_frame_receiver_t;



eltako_frame_receiver_t *eltako_frame_receiver_init();
void eltako_frame_receiver_destroy(eltako_frame_receiver_t *receiver);
void eltako_frame_receiver_add_data(eltako_frame_receiver_t *receiver, uint8_t *data, size_t len);
void eltako_frame_receiver_reset(eltako_frame_receiver_t *receiver);
void eltako_frame_receiver_trim_buffer(eltako_frame_receiver_t *receiver);
eltako_frame_t *eltako_frame_receiver_parse_buffer(eltako_frame_receiver_t *receiver);

#endif /* FRAME_RECEIVER_H_ */
