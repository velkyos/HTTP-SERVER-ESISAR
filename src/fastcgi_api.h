#ifndef FASTCGI_API_H
    #define FASTCGI_API_H

	#include "config.h"

	void fastcgi_request(char *file_name, int request_id, int port, Website *_site, int isPost);

	char * fastcgi_get_body(int *len);

	char * fastcgi_get_type();

	int fastcgi_get_error();

#endif






