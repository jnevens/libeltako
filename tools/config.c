/*
 * config.c
 *
 *  Created on: Mar 28, 2018
 *      Author: jnevens
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "config.h"


bool config_file_parse(config_line_parcer_cb_t cb, const char *config_file, void *arguments)
{
	FILE * fp ;
	char bufr[256];
	char *var = NULL;
	char *val = NULL;
	bool rv = true;

	fp = fopen(config_file, "r");
	if (!fopen) {
		printf("Failed to open config file: %m\n");
	} else {

		while (!feof(fp)) {
			fgets(bufr, sizeof(bufr), fp);

			if (bufr[0] == '#')
				continue;
			if (bufr[0] == '\n')
				continue;
			if (bufr[strlen(bufr) - 1] == '\n')
				bufr[strlen(bufr) - 1] = '\0';
			if ((var = strtok(bufr, "=")) == NULL)
				continue;
			if ((val = strtok(NULL, "=")) == NULL)
				continue;
			if (strlen(var) < 1)
				continue;
			if (strlen(val) < 1)
				continue;

			cb(var, val, arguments);
		}
	}

	fclose(fp);

	return rv;
}

bool config_file_exists(const char *config_file)
{
	if (access(config_file, R_OK) == 0) {
		return true;
	}
	return false;
}
