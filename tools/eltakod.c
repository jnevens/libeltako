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
#include <argp.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <libevquick/libevquick.h>
#include <libvsb/server.h>
#include <libeltako/frame.h>
#include <libeltako/serial.h>
#include <libeltako/frame_receiver.h>

static evquick_event *server_event = NULL;
static evquick_event *eltako_event = NULL;
static eltako_frame_receiver_t *eltako_receiver = NULL;
static int eltako_fd = -1;

/* Used by main to communicate with parse_opt. */
struct arguments
{
	char *dev;
	char *pidfile;
	char *socket;
	bool daemonize;
};

/* Default arguments */
static struct arguments arguments = {
		.dev = "/dev/ttyUSB0",
		.pidfile = "/var/run/eltakod.pid",
		.socket = "/var/run/eltako.socket",
		.daemonize = false,
};

/* Program documentation. */
static char doc[] = "eltakod, a daemon to send and receive telegrams on the eltako series 14 bus.";

/* A description of the arguments we accept. */
static char args_doc[] = "eltakod [options] device";

/* The options we understand. */
static struct argp_option options[] = {
//		{ "device", 'D', "Device", 0, "Device to use (default: /dev/ttyUSB0" },
		{ "pidfile", 'p', "Pidfile", 0, "PID file (default: /var/run/eltakod.pid)" },
		{ "socket", 's', "sockpath", 0, "Unix socket file location (default: /var/run/eltako.socket)" },
		{ "daemonize", 'd', 0, 0, "Run as daemon" },
		{ 0 }
	};

/* Parse a single option. */
static error_t eltako_parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
	 know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
	case 'd':
		arguments->daemonize = true;
		break;
	case 'p':
		arguments->pidfile = arg;
		break;
	case ARGP_KEY_ARG:
		if (state->arg_num == 0)
			arguments->dev = arg;
		else
			argp_usage(state);
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1) {
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, eltako_parse_opt, args_doc, doc };

static int daemon_write_pidfile(const char *pidfile, pid_t pid)
{
	FILE * fp;
	fp = fopen(pidfile, "w+");
	if(!fp){
		fprintf(stderr, "Failed opening PID file!\n");
		return -1;
	}
	if(fprintf(fp,"%d\n",(int)pid) <= 0){
		fprintf(stderr, "Failed writing pid to file!\n");
	}
	fclose(fp);
	return 0;
}

static int daemon_remove_pidfile(const char *pidfile)
{
	return (pidfile) ? unlink(pidfile) : -1;
}

bool daemonize(const char *pf)
{
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Remove pidfile if exists */
	daemon_remove_pidfile(pf);

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(-1);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) { // Child can continue to run even after the parent has finished executing
		printf("Child created: pid: %d\n", pid);
		daemon_write_pidfile(pf, pid);
		exit(0);
	}

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(-1);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		exit(-1);
	}

	return true;
}

void incoming_connection_data_callback(vsb_conn_t *vsb_conn, void *data, size_t len, void *arg)
{
	vsb_server_t *vsb_server = arg;
	printf("Incoming data! (fd = %d)\n", vsb_conn_get_fd(vsb_conn));
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
	/* parse arguments */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if ((eltako_fd = eltako_serial_port_init(arguments.dev)) <= 0) {
		printf("serial port setup failed!\n");
		return -1;
	}

	eltako_serial_port_set_blocking(eltako_fd, false);

	if (arguments.daemonize) {
		if (!daemonize(arguments.pidfile)) {
			fprintf(stderr, "Failed to daemonize!\n");
		}
	}

	evquick_init();
	unlink(arguments.socket);
	vsb_server_t *server = vsb_server_init(arguments.socket);
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
