/*
 * serial.h
 *
 *  Created on: Dec 23, 2015
 *      Author: jnevens
 */

#ifndef SERIAL_H_
#define SERIAL_H_

int eltako_serial_port_init(const char *port);
void eltako_serial_port_close(int fd);
int eltako_serial_read(int fd, void *buf, size_t len);
int eltako_serial_write(int fd, void *buf, size_t len);

#endif /* SERIAL_H_ */
