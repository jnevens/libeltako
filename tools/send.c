#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <argp.h>

#include <libeltako/frame.h>
#include <libeltako/serial.h>
#include <libeltako/frame_receiver.h>
#include <libeltako/message.h>
#include <libeltako/button.h>
#include <libeltako/dimmer.h>

int fd = 0;

/* Used by main to communicate with parse_opt. */
struct arguments
{
	char *dev;
	uint32_t data;
	uint32_t src;
	uint8_t status;
	uint8_t rorg;
};

/* Default arguments */
struct arguments arguments = { .dev = "/dev/ttyUSB0", .status = 0, .data=0x00};

/* Program documentation. */
static char doc[] = "eltako-send, a tool to send telegrams to the eltako series 14 bus.";

/* A description of the arguments we accept. */
static char args_doc[] = "eltako-send [options] source(hex) data(hex)";

/* The options we understand. */
static struct argp_option options[] = {
		{ "device", 'D', "Device", 0, "Device to use (default: /dev/ttyUSB0" },
		{ "status", 's', "Status", 0, "Status (hex, default: 0x00)" },
		{ "rorg", 'r', "RORG", 0, "Rorg (hex, default: 0x05)" },
		{ 0 }
	};

/* Parse a single option. */
static error_t eltako_parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
	 know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
	case 'D':
		arguments->dev = arg;
		break;
	case 's':
		arguments->status = strtoll(arg, NULL, 16);
		break;
	case 'r':
		arguments->rorg = strtoll(arg, NULL, 16);
		break;
	case ARGP_KEY_ARG:
		if (state->arg_num == 0)
			arguments->src = strtoll(arg, NULL, 16);
		else if (state->arg_num == 1)
			arguments->data = strtoll(arg, NULL, 16);
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 2) {
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, eltako_parse_opt, args_doc, doc };



int main(int argc, char *argv[])
{

	/* parse arguments */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
//	if (argc < 3) {
//		printf("usage: %s [PORT] [TYPE] [ACTION] OPTIONAL ARGUMENTS\n", argv[0]);
//		printf("\tPORT    ex.: /dev/ttyUSB0\n");
//		printf("\tTYPE    BUTTON, DIMMER\n");
//		return -1;
//	}

	if ((fd = eltako_serial_port_init(arguments.dev)) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

//	                                     RORG    02    %   DIMS    ON  SOURCE ADDR                ?   CRC
//
//	uint8_t dim[14] = {0xA5, 0x5A ,0x0B ,0x07 ,0x02 ,0x64 ,0x10 ,0x09 ,0xEE ,0xEE ,0xEE ,0x00 ,0x30 ,0xFF};
//	//uint8_t dim[14] = {0xA5, 0x5A ,0x0B ,0x07 ,0x02 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0x00 ,0x00 ,0xFF}; // LEARN
//	eltako_frame_t *ff = frame_create_from_buffer(dim, 14);
//	frame_print(ff);
//	frame_send(ff, fd);
//	usleep(100000);

//	uint8_t btn_on[14] = {0xA5, 0x5A ,0x0B ,0x05 ,0x70 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0xE0 ,0x30 ,0xFF};
//	eltako_frame_t *f = frame_create(btn_on, 14);
//	frame_print(f);
//	frame_send(f, fd);

//	uint8_t btn_off[14] = {0xA5, 0x5A ,0x0B ,0x05 ,0x00 ,0x00 ,0x00 ,0x00 ,0xEE ,0xEE ,0xEE ,0xE0 ,0x20 ,0xFF};
//	eltako_frame_t *fo = frame_create(btn_off, 14);
//	frame_print(fo);
//	frame_send(fo, fd);

//	eltako_message_t *btn_press = button_create_message(0xfefee20e, BTN_WIP_TOP_RIGHT, BTN_EVENT_PRESS);
//	message_print(btn_press);
//	eltako_frame_t *btn_press_frame = message_to_frame(btn_press);
//	frame_print(btn_press_frame);
//	message_send(btn_press, fd);
//	usleep(100000);
//	eltako_message_t *btn_release = button_create_message(0xfefee20e, BTN_WIP_TOP_RIGHT, BTN_EVENT_RELEASE);
//	message_print(btn_release);
//	eltako_frame_t *btn_release_frame = message_to_frame(btn_release);
//	frame_print(btn_release_frame);
//	message_send(btn_release, fd);

	//eltako_message_t *dmr = dimmer_create_message(0xeeeeee00, DIMMER_EVENT_ON, 36, 0x00, false);
	eltako_frame_t *frame = eltako_frame_create(arguments.rorg, (uint8_t *)&arguments.data, arguments.src, arguments.status);
	//message_print(dmr);
	//eltako_frame_t *dmr_frame = message_to_frame(dmr);
	eltako_frame_print(frame);
	eltako_frame_send(frame, fd);

	eltako_serial_port_close(fd);
	return 0;
}
