#ifndef FASTCGI_API_H
    #define FASTCGI_API_H

	void fastcgi_request(char *file_name, int request_id, int port);

	char * fastcgi_getAnswer(char * content_type);

#endif






