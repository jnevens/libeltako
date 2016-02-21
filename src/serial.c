/*
 * serial.c
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#include <libeltako/serial.h>

int eltako_serial_port_init(const char *port)
{
	struct termios tty;

	int fd = open(port, O_RDWR | O_NOCTTY/* | O_NDELAY*/);
	if (fd < 0) {
		perror("open serial port");
		return -1;
	}

	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		perror("tcgetattr");
		return -1;
	}

	cfsetospeed(&tty, B57600);
	cfsetispeed(&tty, B57600);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // ignore break signal
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN] = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
									 // enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= 1;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		perror("tcgetattr");
		return -1;
	}

	return fd;
}

void eltako_serial_port_close(int fd)
{
	close(fd);
}

int eltako_serial_read(int fd, void *buf, size_t len)
{
	return read(fd, buf, len);
}

int eltako_serial_write(int fd, void *buf, size_t len)
{
	return write(fd, buf, len);
}
