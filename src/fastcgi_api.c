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

/* Constants */

#define fastcgi_stdin(fd,id,data,len) fastcgi_send(fd,FCGI_STDIN,id,data,len)
#define fastcgi_data(fd,id,data,len) fastcgi_send(fd,FCGI_DATA,id,data,len)

/* Declaration */

int open_socket(int port);
void sendData_socket(int fd, FCGI_Header *header, int len);
FCGI_Header readData_socket(int fd);

void fastcgi_begin_request(int fd,unsigned short request_id,unsigned short role,unsigned char flags);
void fastcgi_abort_request(int fd,unsigned short request_id);
void fastcgi_params(int fd, unsigned short request_id, const char *file_name, int port, int isPost);

void fastcgi_answer(int fd, unsigned short request_id );
void fastcgi_send(int fd, unsigned char type, unsigned short request_id, char *data, unsigned int len);

int addNameValuePair(FCGI_Header *h,char *name,char *value);
void writeLen(int len, char **p);

/* Definition */

char * fastcgi_get_body(int *len){
	return NULL;
}

char * fastcgi_get_type(){
	return NULL;
}


void fastcgi_request(char *file_name, int request_id, int port, int isPost){
	_Token *body = searchTree(NULL, "message-body");


	int fd = open_socket(port);

	fastcgi_begin_request(fd, request_id, FCGI_RESPONDER, FCGI_KEEP_CONN);

	/*Ajouter condition si on est dans le cas d'un post*/
	fastcgi_params(fd, request_id, file_name, port, isPost);

	if ( body != NULL && isPost == 1)
	{
		int body_length;
		char *body_data = getElementValue(body->node, &body_length);

		fastcgi_stdin(fd, request_id, body_data, body_length);
		fastcgi_stdin(fd, request_id, "", 0);
	}
	

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
	return fd;
}

FCGI_Header readData_socket(int fd){
	FCGI_Header header;
	int size = 0;
	int n = 0;

	while ( n < FCGI_HEADER_SIZE)
	{
		do {
			size += read(fd, &header + n , FCGI_HEADER_SIZE - n);
		} while (size == -1 && errno == EINTR);
		n += size;
	}

	if (n != FCGI_HEADER_SIZE) printf("FCGI -> ERREUR Read\n");

	size = 0;
	n = 0;

	int data_length = ntohs(header.contentLength);
	int padding_length = header.paddingLength;

	printf("\n\nLecture de %d : %d et de %d\n", header.type, data_length, header.paddingLength);
	while ( n < data_length)
	{
		do {
			size = read(fd, header.contentData + n, data_length - n);
		} while (size == -1 && errno == EINTR);
		n += size;
		printf("lu %d char\n", size);
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
	printf("J'ai lu que %d\n", n);
	return header;
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
	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

void fastcgi_params(int fd, unsigned short request_id, const char *file_name, int port, int isPost){
	FCGI_Header h;

	h.version = FCGI_VERSION_1;
	h.type = FCGI_PARAMS;
	h.requestId = htons(request_id);
	h.paddingLength = 0;
	h.contentLength = 0;
	
	addNameValuePair(&h,"SCRIPT_FILENAME","proxy:fcgi://127.0.0.1:9000//var/tes/test.php");
	addNameValuePair(&h,"SERVER_PORT","80");
	addNameValuePair(&h,"SERVER_NAME","127.0.0.1");
	addNameValuePair(&h,"SERVER_ADDR","127.0.0.1");
	addNameValuePair(&h,"DOCUMENT_ROOT","/var/tes");
	addNameValuePair(&h,"GATEWAY_INTERFACE","CGI/1.1");
	addNameValuePair(&h,"SERVER_PROTOCOLE","HTTP/1.1");
	addNameValuePair(&h,"QUERY_STRING",NULL);
	if( isPost) addNameValuePair(&h,"REQUEST_METHOD","POST");
	else addNameValuePair(&h,"REQUEST_METHOD","GET");
	addNameValuePair(&h,"REQUEST_URL","test.php");
	addNameValuePair(&h,"SCRIPT_NAME","test.php");

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
	if(data != NULL) memcpy(h.contentData,data,len); 

	sendData_socket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));  

}

void fastcgi_answer(int fd, unsigned short request_id ){
	FCGI_Header header;
	header.type = FCGI_UNKNOWN_TYPE;

	do
	{
		header = readData_socket(fd);
	} while ( header.type == FCGI_STDOUT );
	
	if ( header.type != FCGI_END_REQUEST) printf("ERREUR MAUVAIS TYPE\n");
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
