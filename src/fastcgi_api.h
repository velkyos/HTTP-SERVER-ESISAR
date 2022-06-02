#ifndef FASTCGI_API_H
    #define FASTCGI_API_H
	/**
	 * @file fastcgi_api.h
	 * @author ROBERT Benjamin & PEDER Leo & MANDON Anaël
	 * @brief Interface between HTTP and FASTCGI
	 * @version 0.8
	 * @date 2022-05-30
	 */

	/* System Includes */
	
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <time.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>

	/* User Includes */

	#include "config.h"
	#include "fastcgi.h" 
	#include "syntax_api.h"
	#include "process_api.h"
	#include "utils.h"

	#define fastcgi_stdin(fd,id,data,len) fastcgi_send(fd,FCGI_STDIN,id,data,len)
	#define fastcgi_data(fd,id,data,len) fastcgi_send(fd,FCGI_DATA,id,data,len)

	typedef struct st_fastcgi_stdout_link
	{
		FCGI_Header *header;
		struct st_fastcgi_stdout_link *next;
	}Fastcgi_stdout_linked;

	
	void fastcgi_request(char *file_name, int request_id, int port, Website *_site, int isPost);

	int open_socket(int port);
	void sendData_socket(int fd, FCGI_Header *header, int len);

	int addNameValuePair(FCGI_Header *h,char *name,char *value);
	void writeLen(int len, char **p);

	void fastcgi_send(int fd, unsigned char type, unsigned short request_id, char *data, unsigned int len);
	void fastcgi_begin_request(int fd,unsigned short request_id,unsigned short role,unsigned char flags);
	void fastcgi_abort_request(int fd,unsigned short request_id);

	void fastcgi_params(int fd, unsigned short request_id,char *file_name, int port, int isPost);
	void try_add_params(FCGI_Header *h, char *name, char *value);
	void add_params_script(FCGI_Header *h, char *file_name, int port);
	void add_params_query(FCGI_Header *h);
	void add_params_name(FCGI_Header *h);

	void fastcgi_answer(int fd, unsigned short request_id );
	FCGI_Header *readData_socket(int fd);
	char *fastcgi_concat_stdout(Fastcgi_stdout_linked *list, int * len);
	void handle_fastcgi_data(char * _data, int _len);

	void add_stdout_list(Fastcgi_stdout_linked **list, FCGI_Header *header);
	void purge_stdout_list(Fastcgi_stdout_linked **list);
	FCGI_Header *get_stdout_list(Fastcgi_stdout_linked *list, int i);

	char * fastcgi_get_body(int *len);

	char * fastcgi_get_type();

	int fastcgi_get_error();

#endif






