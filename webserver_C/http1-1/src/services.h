#ifndef _SERVICES_H_
#define _SERVICES_H_

#include "common.h"

struct client_request {
	int client_fd;
	char *filepath;
	char *filetype;
};

int client_services(int client_fd);

#endif