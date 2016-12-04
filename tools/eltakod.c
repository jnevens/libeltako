/*
 * eltakod.c
 *
 *  Created on: Oct 18, 2016
 *      Author: jnevens
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <libevquick/libevquick.h>
#include <libvsb/server.h>
#include <libeltako/frame.h>
#include <libeltako/serial.h>
#include <libeltako/frame_receiver.h>

static evquick_event *server_event = NULL;
static evquick_event *eltako_event = NULL;
static eltako_frame_receiver_t *eltako_receiver = NULL;
static int eltako_fd = -1;

void incoming_connection_data_callback(vsb_conn_t *vsb_conn, void *data, size_t len, void *arg)
{
	vsb_server_t *vsb_server = arg;
	printf("Incoming data! (fd = %d)", vsb_conn_get_fd(vsb_conn));
	eltako_frame_t *frame = eltako_frame_create_from_buffer(data, len);
	vsb_server_broadcast(vsb_server, data, len, vsb_conn);
	eltako_frame_send(frame, eltako_fd);
}

void incoming_connection_callback(int fd, short revents, void *arg)
{
	vsb_conn_t *vsb_conn = (vsb_conn_t *) arg;

	vsb_server_handle_connection_event(vsb_conn);
}

void incoming_vsb_server_event(int fd, short revents, void *arg)
{
	vsb_server_t *vsb_server = (vsb_server_t *) arg;
	printf("incoming connection! \n");
	vsb_server_handle_server_event(vsb_server);
}

void connection_disconnect_callback(vsb_conn_t *conn, void *arg)
{
	evquick_event *event = (evquick_event *)arg;
	fprintf(stderr, "Disconnecting client! (fd = %d)\n", event->fd);
	evquick_delevent(event);
}

void new_connection_callback(vsb_conn_t *vsb_conn, void *arg)
{
	fprintf(stderr, "new connection callback (fd = %d)\n", vsb_conn_get_fd(vsb_conn));
	evquick_event *event = evquick_addevent(vsb_conn_get_fd(vsb_conn), EVQUICK_EV_READ, incoming_connection_callback, NULL,
			vsb_conn);
	vsb_conn_register_disconnect_cb(vsb_conn, connection_disconnect_callback, (void *)event);
}

void incoming_eltako_data(int fd, short revents, void *arg)
{
	vsb_server_t *server = arg;

	fprintf(stdout, "incoming eltako data!");

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
			eltako_frame_receiver_add_data(eltako_receiver, buf, (size_t) rval);

			eltako_frame_t *frame = NULL;
			while ((frame = eltako_frame_receiver_parse_buffer(eltako_receiver)) != NULL) {
				uint8_t *data = eltako_frame_get_data(frame);
				size_t data_size = eltako_frame_get_raw_size(frame);

				vsb_server_send(server, data, data_size);

				eltako_frame_print(frame);
				eltako_frame_destroy(frame);
			}
		}
	}

}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s [PORT]\n", argv[0]);
		return -1;
	}

	if ((eltako_fd = eltako_serial_port_init(argv[1])) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

	evquick_init();
	const char *path = "/var/run/eltako.socket";
	unlink(path);
	vsb_server_t *server = vsb_server_init(path);
	vsb_server_set_auto_broadcast(server, false);
	vsb_server_register_new_connection_cb(server, new_connection_callback, NULL);
	vsb_server_register_receive_data_cb(server, incoming_connection_data_callback, server);
	fprintf(stderr, "VSB server (fd = %d)\n", vsb_server_get_fd(server));
	server_event = evquick_addevent(vsb_server_get_fd(server), EVQUICK_EV_READ, incoming_vsb_server_event, NULL, server);

	eltako_receiver = eltako_frame_receiver_init();
	eltako_event = evquick_addevent(eltako_fd, EVQUICK_EV_READ, incoming_eltako_data, NULL, server);

	evquick_loop();

	evquick_delevent(server_event);
	vsb_server_close(server);
	eltako_serial_port_close(eltako_fd);
	eltako_frame_receiver_destroy(eltako_receiver);
	return 0;
}
