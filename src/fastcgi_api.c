/**
 * @file fastcgi_api.c
 * @author ROBERT Benjamin & PEDER Leo & MANDON Anaël
 * @brief Interface between HTTP and FASTCGI
 * @version 0.1
 * @date 2022-05-30
 */
/* System Includes */

#include <stdio.h>
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

#include "fastcgi.h" 
#include "fastcgi_api.h" 
#include "syntax_api.h"
#include "process_api.h"
#include "utils.h"

/* Constants */

#define fastcgi_stdin(fd,id,data,len) fastcgi_send(fd,FCGI_STDIN,id,data,len)
#define fastcgi_data(fd,id,data,len) fastcgi_send(fd,FCGI_DATA,id,data,len)

/* Declaration */

typedef struct st_fastcgi_stdout_link
{
	FCGI_Header *header;
	struct st_fastcgi_stdout_link *next;
}Fastcgi_stdout_linked;

int open_socket(int port);
void sendData_socket(int fd, FCGI_Header *header, int len);

int addNameValuePair(FCGI_Header *h,char *name,char *value);
void writeLen(int len, char **p);

void fastcgi_send(int fd, unsigned char type, unsigned short request_id, char *data, unsigned int len);
void fastcgi_begin_request(int fd,unsigned short request_id,unsigned short role,unsigned char flags);
void fastcgi_abort_request(int fd,unsigned short request_id);

void fastcgi_params(int fd, unsigned short request_id,char *file_name, int port, int isPost);
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

void try_add_params(FCGI_Header *h, char *name, char *value);


/* Definition */

int fastcgi_error;
char *fastcgi_data = NULL;
char *fastcgi_type = NULL;
int fastcgi_length = 0;
Website *site = NULL;

char * fastcgi_get_body(int *len){
	if (len) *len = fastcgi_length;
	return fastcgi_data;
}

char * fastcgi_get_type(){
	return fastcgi_type;
}

int fastcgi_get_error(){
	return fastcgi_error;
}

void fastcgi_request(char *file_name, int request_id, int port, Website *_site, int isPost){
	site = _site;
	fastcgi_data = NULL;
	fastcgi_length = 0;
	fastcgi_type = NULL;
	fastcgi_error = 200;

	char *body = get_header_val("message-body");

	int fd = open_socket(port);

	fastcgi_begin_request(fd, request_id, FCGI_RESPONDER, FCGI_KEEP_CONN);

	/*Ajouter condition si on est dans le cas d'un post*/
	fastcgi_params(fd, request_id, file_name, port, isPost);

	if ( body != NULL && isPost == 1)
	{
		fastcgi_stdin(fd, request_id, body, strlen(body));
		
		free(body);
	}

	fastcgi_send(fd, FCGI_STDIN, request_id, NULL, 0);

	fastcgi_answer(fd, request_id);
}

int open_socket(int port){
	int fd;
	struct sockaddr_in serv_addr;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed\n");
		return (-1);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed\n");
		return (-1);
	}
	/*
	struct sockaddr_in sin;	
	socklen_t len = sizeof(sin);
	if (getsockname(fd, (struct sockaddr *)&sin, &len) == -1)
		perror("getsockname");
	else
		printf("port number %d\n", ntohs(sin.sin_port));

*/
	return fd;
}

void sendData_socket(int fd, FCGI_Header *header, int len){
	header->contentLength=htons(header->contentLength); 
	header->paddingLength=htons(header->paddingLength); 

	int size;
	while ( len > 0 ){
		do {
			size = write(fd, header, len);
		} while (size == -1 && errno == EINTR);
		len -= size;
		header += size;
	}
}

void fastcgi_begin_request(int fd,unsigned short request_id,unsigned short role,unsigned char flags){
	FCGI_Header h; 
	FCGI_BeginRequestBody *begin; 

	h.version=FCGI_VERSION_1; 
	h.type=FCGI_BEGIN_REQUEST; 
	h.requestId=htons(request_id); 
	h.contentLength=sizeof(FCGI_BeginRequestBody); 
	h.paddingLength=0; 
	h.reserved = 0;
	memset(h.contentData, '\0', FASTCGILENGTH);

	begin=(FCGI_BeginRequestBody *)&(h.contentData); 
	begin->role=htons(role); 
	begin->flags=flags; 
	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

void fastcgi_abort_request(int fd,unsigned short request_id){
	FCGI_Header h; 

	h.version=FCGI_VERSION_1; 
	h.type=htons(FCGI_ABORT_REQUEST); 
	h.requestId=request_id; 
	h.contentLength=0; 
	h.paddingLength=0; 
	h.reserved = 0;
	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

void fastcgi_params(int fd, unsigned short request_id, char *file_name, int port, int isPost){
	FCGI_Header h;

	h.version = FCGI_VERSION_1;
	h.type = FCGI_PARAMS;
	h.requestId = htons(request_id);
	h.paddingLength = 0;
	h.contentLength = 0;
	h.reserved = 0;
	
	//?HOST
	try_add_params(&h, "HTTP_HOST", "Host");
	//?USER_AGENT
	try_add_params(&h, "HTTP_USER_AGENT", "User-Agent");
	//?ACCEPT
	try_add_params(&h, "HTTP_ACCEPT", "Accept");
	//?ACCEPT_LANGUAGE
	try_add_params(&h, "HTTP_ACCEPT_LANGUAGE", "Accept-Language");
	//?ACCEPT_ENCODING
	try_add_params(&h, "HTTP_ACCEPT_ENCODING", "Accept-Encoding");
	//?CONNECTION
	try_add_params(&h, "HTTP_CONNECTION", "Connection");

	if( isPost ){
		//>CONTENT_TYPE
		try_add_params(&h, "CONTENT_TYPE", "Content-Type");
		//>CONTENT_LENGTH
		try_add_params(&h, "CONTENT_LENGTH", "Content-Length");
		//>REFERER
		try_add_params(&h, "HTTP-REFERERdqsd", "Referer");
	}
	
	//ADDRESS
	//PORT
	//REMOTEPORT


	//DOCUMENT_ROOT
	addNameValuePair(&h,"DOCUMENT_ROOT", site->root);
	//SCRIPTFILENAME
	add_params_script(&h, file_name, port);
	//URI
	try_add_params(&h, "REQUEST_URI", "request-target");
	//SCRIPTNAME
	char *temp = get_file_name();
	addNameValuePair(&h, "SCRIPT_NAME", temp);
	free(temp);

	//INTERFACE
	addNameValuePair(&h, "GATEWAY_INTERFACE","CGI/1.1");
	//PROTOCAOLE ?maybe faire gaffe au 1.0
	addNameValuePair(&h, "SERVER_PROTOCOL","HTTP/1.1");

	//METHODE
	if( isPost) addNameValuePair(&h,"REQUEST_METHOD","POST");
	else addNameValuePair(&h,"REQUEST_METHOD","GET");
	//QUERY
	add_params_query(&h);
	//REQUEST_SCHEME
	addNameValuePair(&h, "REQUEST_SCHEME","http");

	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));  
	fastcgi_send(fd, FCGI_PARAMS, request_id, NULL, 0);

}

void fastcgi_send(int fd, unsigned char type, unsigned short request_id, char *data, unsigned int len) {
	FCGI_Header h; 

	if (len > FASTCGILENGTH) {printf("ERROR\n");return ; }

	h.version=FCGI_VERSION_1; 
	h.type=type; 
	h.requestId=htons(request_id); 
	h.contentLength=len;
	h.paddingLength=0;
	h.reserved = 0;
	if(data != NULL) memcpy(h.contentData,data,len);
	else memset(h.contentData, '\0', FASTCGILENGTH);

	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));  

}

void writeLen(int len, char **p) {
	if (len > 0x7F ) { 
		*((*p)++)=(len>>24)&0x7F; 
		*((*p)++)=(len>>16)&0xFF; 
		*((*p)++)=(len>>8)&0xFF; 
		*((*p)++)=(len)&0xFF; 
	} else *((*p)++)=(len)&0x7F;
}
	
int addNameValuePair(FCGI_Header *h,char *name,char *value)
{
	char *p; 
	unsigned int nameLen=0,valueLen=0;

	if (name) nameLen=strlen(name); 
	if (value) valueLen=strlen(value);

	if ((valueLen > FASTCGIMAXNVPAIR) || (valueLen > FASTCGIMAXNVPAIR) ) return -1; 
	if ((h->contentLength+((nameLen>0x7F)?4:1)+((valueLen>0x7F)?4:1)) > FASTCGILENGTH ) return -1; 
	
	p=(h->contentData)+h->contentLength; 
	writeLen(nameLen,&p); 
	writeLen(valueLen,&p);
	strncpy(p,name,nameLen); 
	p+=nameLen; 
	if (value) strncpy(p,value,valueLen); 
	h->contentLength+=nameLen+((nameLen>0x7F)?4:1);
	h->contentLength+=valueLen+((valueLen>0x7F)?4:1);
	return 0;
}	 

/*Output */

FCGI_Header *readData_socket(int fd){
	FCGI_Header *header = malloc( FASTCGILENGTH + FCGI_HEADER_SIZE);
	header->version=0; 
	header->type=0; 
	header->requestId=0; 
	header->contentLength=0; 
	header->paddingLength=0; 
	header->reserved = 0;
	memset(header->contentData, '\0', FASTCGILENGTH);

	int size = 0;
	int n = 0;

	while ( n < FCGI_HEADER_SIZE)
	{
		do {
			size += read(fd, header + n , FCGI_HEADER_SIZE - n);
		} while (size == -1 && errno == EINTR);
		n += size;
	}

	if (n != FCGI_HEADER_SIZE) printf("FCGI -> ERREUR Read\n");

	size = 0;
	n = 0;

	int data_length = ntohs(header->contentLength);
	int padding_length = header->paddingLength;


	while ( n < data_length)
	{
		do {
			size = read(fd, header->contentData + n, data_length - n);
		} while (size == -1 && errno == EINTR);
		n += size;

	}

	size = 0;
	n = 0;
	char temp;
	
	while ( n < padding_length)
	{
		do {
			size = read(fd, &temp, 1);
		} while (size == -1 && errno == EINTR);
		n += size;
	}

	return header;
}

void fastcgi_answer(int fd, unsigned short request_id ){
	Fastcgi_stdout_linked *list = NULL;
	int i = -1;

	do
	{
		i++;
		add_stdout_list(&list, readData_socket(fd));

	} while ( get_stdout_list(list, i)->type == FCGI_STDOUT || get_stdout_list(list, i)->type == FCGI_STDERR );
	
	int len;
	char * data = fastcgi_concat_stdout( list, &len);

	handle_fastcgi_data( data, len);

	free(data);
	purge_stdout_list(&list);
}

void handle_fastcgi_data(char * _data, int _len){
	char *error = strstr(_data, "Status: ");
	char *type = strstr(_data, "Content-type:");
	char *data = strstr(_data, "\r\n\r\n") + 2;
	int len;

	fastcgi_data = NULL;

	if(error) fastcgi_error = atoi(error + 8);
	
	if( type == 0 || data == 0) return;

	len = (int)(data - type);

	fastcgi_type = malloc( len + 1);
	memset( fastcgi_type, '\0', len + 1);
	memcpy( fastcgi_type, type, len );

	len = _len - (int)(data - _data);

	fastcgi_length = len;
	fastcgi_data = malloc( len + 1);
	memset( fastcgi_data, '\0', len + 1);
	memcpy( fastcgi_data, data, len );
}

char *fastcgi_concat_stdout(Fastcgi_stdout_linked *list, int * len){
	Fastcgi_stdout_linked *temp = list;
	int size = 0;

	while ( temp != NULL)
	{		
		if( temp->header->type == FCGI_STDERR) printf("\n WARNING ! \n %s \n", temp->header->contentData);

		if( temp->header->type == FCGI_STDOUT) size += ntohs( temp->header->contentLength);
			
		temp = temp->next;
	}
	
	int index = 0;
	temp = list;
	*len = size;

	char *result = malloc(size + 1);
	memset(result, '\0', size + 1);

	while ( temp != NULL)
	{
		if( temp->header->type == FCGI_STDOUT){
			int tempSize = ntohs( temp->header->contentLength);
			memcpy( result + index, temp->header->contentData, tempSize );
			index += tempSize;
		}

		temp = temp->next;

	}


	return result;
}

void add_stdout_list(Fastcgi_stdout_linked **list, FCGI_Header *header){
	Fastcgi_stdout_linked *new = malloc(sizeof(Fastcgi_stdout_linked));
	new->header = header;
	new->next = NULL;

	Fastcgi_stdout_linked *temp = *list;

	if( temp == NULL) *list = new;
	else{
		while ( temp->next != NULL ) temp = temp->next;

		temp->next = new;
	}
}

void purge_stdout_list(Fastcgi_stdout_linked **list){
	Fastcgi_stdout_linked *temp = *list;
	Fastcgi_stdout_linked *current = NULL;

	if (*list != NULL){
		while( temp != NULL){
			current = temp;
			temp = temp->next;
			free(current->header);
			free(current);
		}
	}
}

FCGI_Header *get_stdout_list(Fastcgi_stdout_linked *list, int i){
	Fastcgi_stdout_linked *temp = list;
	int j = 0;

	if( temp != NULL){
		while ( temp != NULL && j++ < i) temp = temp->next;
	}
	return temp->header;
}


/* PARAMS */

void add_params_query(FCGI_Header *h){
	_Token *query = searchTree(NULL, "query");
	if (query){
		int len;
		char *temp = getElementValue(query->node, &len);
		char *query_string = malloc( len + 1);
		memset(query_string, '\0', len + 1);
		memcpy(query_string, temp, len);
		addNameValuePair(h,"QUERY_STRING", query_string);
		free(query_string);
	} 
	else addNameValuePair(h,"QUERY_STRING",NULL);
}

void add_params_script(FCGI_Header *h, char *file_name, int port){
	int len = 32 + strlen(file_name);
	char *test = malloc( len );

	memset(test, '\0', len);

	sprintf(test, "proxy:fcgi://127.0.0.1:%d/%s", port, file_name);

	if ( strlen(test) > 127){
		printf(" --> WARNING : file path too big, place the file in another folder closer to the root !\n");
	}

	addNameValuePair(h,"SCRIPT_FILENAME", test);
	free(test);
}

void try_add_params(FCGI_Header *h, char *name, char *value){
	char * val = get_header_val(value);
	if(val) 
	{
		addNameValuePair(h, name, val);
		free(val);
	}
	
}
