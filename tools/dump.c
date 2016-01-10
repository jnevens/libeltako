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

static int fd = 0;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s [PORT]\n", argv[0]);
		return -1;
	}

	if ((fd = serial_port_init(argv[1])) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

	frame_receiver_t *receiver =	frame_receiver_init();
	uint8_t buf[1024];
	ssize_t rval;

	bzero(buf, sizeof(buf));
	while (1) {
		if ((rval = serial_read(fd, buf, sizeof(buf))) < 0) {
			if (errno == EWOULDBLOCK) {
				break;
			}
			perror("reading stream message");
		} else if (rval > 0) { // get data
			frame_receiver_add_data(receiver, buf, (size_t) rval);

			frame_t *frame = NULL;
			while ((frame = frame_receiver_parse_buffer(receiver)) != NULL) {
				frame_print(frame);
				message_t *msg = message_create_from_frame(frame);
				message_print(msg);
				frame_destroy(frame);
				message_destroy(msg);
			}
		}
	}

	serial_port_close(fd);
	frame_receiver_destroy(receiver);
	return 0;
}
