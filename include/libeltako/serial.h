/*
 * serial.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef SERIAL_H_
#define SERIAL_H_

int serial_port_init(char *port);
void serial_port_close(int fd);
int serial_read(int fd, void *buf, size_t len);
int serial_write(int fd, void *buf, size_t len);

#endif /* SERIAL_H_ */
