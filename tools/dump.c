#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#include <libeltako/frame.h>
#include <libeltako/serial.h>
#include <libeltako/frame_receiver.h>
#include <libeltako/message.h>

static int fd = 0;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s [PORT]\n", argv[0]);
		return -1;
	}

	if ((fd = eltako_serial_port_init(argv[1])) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

	eltako_frame_receiver_t *receiver =	eltako_frame_receiver_init();
	uint8_t buf[1024];
	ssize_t rval;

	bzero(buf, sizeof(buf));
	while (1) {
		if ((rval = eltako_serial_read(fd, buf, sizeof(buf))) < 0) {
			if (errno == EWOULDBLOCK) {
				break;
			}
			perror("reading stream message");
		} else if (rval > 0) { // get data
			eltako_frame_receiver_add_data(receiver, buf, (size_t) rval);

			eltako_frame_t *frame = NULL;
			while ((frame = eltako_frame_receiver_parse_buffer(receiver)) != NULL) {
				eltako_frame_print(frame);
				eltako_message_t *msg = eltako_message_create_from_frame(frame);
				eltako_message_print(msg);
				eltako_frame_destroy(frame);
				eltako_message_destroy(msg);
			}
		}
	}

	eltako_serial_port_close(fd);
	eltako_frame_receiver_destroy(receiver);
	return 0;
}
