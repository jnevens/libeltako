/*
 * config.h
 *
 *  Created on: Mar 28, 2018
 *      Author: jnevens
 */

#ifndef TOOLS_CONFIG_H_
#define TOOLS_CONFIG_H_

#include <stdbool.h>

typedef int (config_line_parcer_cb_t)(const char *arg, const char *val, void *arguments);

bool config_file_parse(config_line_parcer_cb_t cb, const char *config_file, void *arguments);
bool config_file_exists(const char *config_file);

#endif /* TOOLS_CONFIG_H_ */
