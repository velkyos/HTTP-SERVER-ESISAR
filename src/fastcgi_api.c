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

#define fastcgi_stdin(fd,id,stdin,len) fastcgi_send(fd,FCGI_STDIN,id,stdin,len)
#define fastcgi_data(fd,id,data,len) fastcgi_send(fd,FCGI_DATA,id,data,len)

/* Declaration */

int open_socket(int port);
void sendData_socket(int socket, FCGI_Header *header, int len);

void fastcgi_begin_request();
void fastcgi_abort_request();
void fastcgi_end_request();
void fastcgi_params();
void fastcgi_stdout();
void fastcgi_stderr();
void fastcgi_get_values();
void fastcgi_get_values_result();

void fastcgi_send(int fd, unsigned char type, unsigned short requestId, char *data, unsigned int len);

/* Definition */


void fastcgi_request(char *fileName){
	_Token tok;
}


int open_socket(int port){
	int fd;
	struct sockaddr_in serv_addr;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation failed\n");
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

void sendData_socket(int socket, FCGI_Header *header, int len){
	header->contentLength=htons(header->contentLength); 
	header->paddingLength=htons(header->paddingLength); 

	int size;

	while ( len > 0 ){
		do {
			size = write(socket, header, len);
		} while (size == -1 && errno == EINTR);
		len -= size;
	}
}

void fastcgi_begin_request(int fd,unsigned short requestId,unsigned short role,unsigned char flags){
	FCGI_Header h; 
	FCGI_BeginRequestBody *begin; 

	h.version=FCGI_VERSION_1; 
	h.type=FCGI_BEGIN_REQUEST; 
	h.requestId=htons(requestId); 
	h.contentLength=sizeof(FCGI_BeginRequestBody); 
	h.paddingLength=0; 
	begin=(FCGI_BeginRequestBody *)&(h.contentData); 
	begin->role=htons(role); 
	begin->flags=flags; 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

void fastcgi_abort_request(int fd,unsigned short requestId){
	FCGI_Header h; 

	h.version=FCGI_VERSION_1; 
	h.type=htons(FCGI_ABORT_REQUEST); 
	h.requestId=requestId; 
	h.contentLength=0; 
	h.paddingLength=0; 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

void fastcgi_end_request(int fd,unsigned short requestId){

}

void fastcgi_params(int fd, unsigned short requestId){

}

void fastcgi_stdout(int fd, unsigned short requestId){

}

void fastcgi_stderr(int fd, unsigned short requestId){

}

void fastcgi_get_values(int fd, unsigned short requestId){

}

void fastcgi_get_values_result(int fd, unsigned short requestId){

}

void fastcgi_send(int fd, unsigned char type, unsigned short requestId, char *data, unsigned int len) {
	FCGI_Header h; 

	if (len > FASTCGILENGTH) return ; 
	h.version=FCGI_VERSION_1; 
	h.type=type; 
	h.requestId=htons(requestId); 
	h.contentLength=len; 
	h.paddingLength=0;
	memcpy(h.contentData,data,len); 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));  
}


























/*
// =========================================================================================================== // 
void writeLen(int len, char **p) {
	if (len > 0x7F ) { 
		*((*p)++)=(len>>24)&0x7F; 
		*((*p)++)=(len>>16)&0xFF; 
		*((*p)++)=(len>>8)&0xFF; 
		*((*p)++)=(len)&0xFF; 
	} else *((*p)++)=(len)&0x7F;
}
	
// =========================================================================================================== // 
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
}	 
// =========================================================================================================== // 		
	
void sendGetValue(int fd) 
{
FCGI_Header h; 

	h.version=FCGI_VERSION_1; 
	h.type=FCGI_GET_VALUES; 
	h.requestId=htons(FCGI_NULL_REQUEST_ID); 
	h.contentLength=0; 
	h.paddingLength=0; 
	addNameValuePair(&h,FCGI_MAX_CONNS,NULL); 
	addNameValuePair(&h,FCGI_MAX_REQS,NULL); 
	addNameValuePair(&h,FCGI_MPXS_CONNS,NULL); 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}
=============================================================================================== // 
int main(int argc,char *argv[])
{
	int fd; 
	fd=createSocket(9000); 
	sendGetValue(fd); 
	sendBeginRequest(fd,10,FCGI_RESPONDER,FCGI_KEEP_CONN); 
	sendStdin(fd,10,argv[1],strlen(argv[1])); 
	sendData(fd,10,argv[1],strlen(argv[1])); 
}

*/