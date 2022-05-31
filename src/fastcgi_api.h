#ifndef FASTCGI_API_H
    #define FASTCGI_API_H

	void fastcgi_request(char *file_name, int request_id, int port, int isPost);

	char * fastcgi_get_body(int *len);

	char * fastcgi_get_type();

#endif






