#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#include "frame.h"
#include "serial.h"
#include "frame_receiver.h"
#include "message.h"
#include "button.h"
#include "dimmer.h"

int fd = 0;

void print_buf(uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		printf("%02X ", buf[i]);
	}
	printf("\n");
}



int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("usage: %s [PORT] [TYPE] [ACTION] OPTIONAL ARGUMENTS\n", argv[0]);
		printf("\tPORT    ex.: /dev/ttyUSB0\n");
		printf("\tTYPE    BUTTON, DIMMER\n");
		return -1;
	}

	if ((fd = serial_port_init(argv[1])) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

//	                                     RORG    02    %   DIMS    ON  SOURCE ADDR                ?   CRC
//
//	uint8_t dim[14] = {0xA5, 0x5A ,0x0B ,0x07 ,0x02 ,0x64 ,0x10 ,0x09 ,0xEE ,0xEE ,0xEE ,0x00 ,0x30 ,0xFF};
//	//uint8_t dim[14] = {0xA5, 0x5A ,0x0B ,0x07 ,0x02 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0x00 ,0x00 ,0xFF}; // LEARN
//	frame_t *ff = frame_create_from_buffer(dim, 14);
//	frame_print(ff);
//	frame_send(ff, fd);
//	usleep(100000);

//	uint8_t btn_on[14] = {0xA5, 0x5A ,0x0B ,0x05 ,0x70 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0xE0 ,0x30 ,0xFF};
//	frame_t *f = frame_create(btn_on, 14);
//	frame_print(f);
//	frame_send(f, fd);

//	uint8_t btn_off[14] = {0xA5, 0x5A ,0x0B ,0x05 ,0x00 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0xE0 ,0x20 ,0xFF};
//	frame_t *fo = frame_create(btn_off, 14);
//	frame_print(fo);
//	frame_send(fo, fd);

//	message_t *btn_press = button_create_message(0xfefee20e, BTN_WIP_TOP_RIGHT, BTN_EVENT_PRESS);
//	message_print(btn_press);
//	frame_t *btn_press_frame = message_to_frame(btn_press);
//	frame_print(btn_press_frame);
//	message_send(btn_press, fd);
//	usleep(100000);
//	message_t *btn_release = button_create_message(0xfefee20e, BTN_WIP_TOP_RIGHT, BTN_EVENT_RELEASE);
//	message_print(btn_release);
//	frame_t *btn_release_frame = message_to_frame(btn_release);
//	frame_print(btn_release_frame);
//	message_send(btn_release, fd);

	message_t *dmr = dimmer_create_message(0xeeeeee00, DIMMER_EVENT_ON, 36, 0x00, false);
	message_print(dmr);
	frame_t *dmr_frame = message_to_frame(dmr);
	frame_print(dmr_frame);
	message_send(dmr, fd);

	serial_port_close(fd);
	return 0;
}
