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

	/**
	 * @brief A Simple structure for storing stdout message.
	 */
	typedef struct st_fastcgi_stdout_link
	{
		FCGI_Header *header;
		struct st_fastcgi_stdout_link *next;
	}Fastcgi_stdout_linked;

	/**
	 * @brief 
	 * 
	 * @param file_name The complete path of the target file.
	 * @param request_id A number use to tell which request we are in.
	 * @param port The port used by the PHP server.
 	 * @param _site The current website being shown.
	 * @param isPost If the request is a Post or not.
	 */
	void fastcgi_request(char *file_name, int request_id, int port, Website *_site, int isPost);

	/**
	 * @brief Open a socket based on the current port.
	 * 
	 * @param port The port used by the PHP server.
	 * @return The id of the current tube.
	 */
	int open_socket(int port);

	/**
	 * @brief Send data throw the socket to the php server.
	 * 
	 * @param fd The id of the current tube.
	 * @param header The pointer to the current header.
	 * @param len The length of the Header + data + padding.
	 */
	void sendData_socket(int fd, FCGI_Header *header, int len);

	int addNameValuePair(FCGI_Header *h,char *name,char *value);
	
	void writeLen(int len, char **p);

	/**
	 * @brief Generate an Header based on this parameters and send it.
	 * 
	 * @param fd The id of the current tube.
	 * @param type The type of the message.
	 * @param request_id A number use to tell which request we are in.
	 * @param data The data for this message.
	 * @param len The length of the data.
	 */
	void fastcgi_send(int fd, unsigned char type, unsigned short request_id, char *data, unsigned int len);
	
	/**
	 * @brief Generate a bing request.
	 * 
	 * @param fd The id of the current tube.
	 * @param request_id A number use to tell which request we are in.
	 * @param role Role variable use for the body of the request.
	 * @param flags Flags used for the body of the request.
	 */
	void fastcgi_begin_request(int fd,unsigned short request_id,unsigned short role,unsigned char flags);
	
	/**
	 * @brief Generate an abort request.
	 * 
	 * @param fd The id of the current tube.
	 * @param request_id A number use to tell which request we are in.
	 */
	void fastcgi_abort_request(int fd,unsigned short request_id);

	/**
	 * @brief Handle params generation
	 * 
	 * @param fd The id of the current tube.
	 * @param request_id A number use to tell which request we are in.
	 * @param file_name The complete path of the target file.
	 * @param port The port used by the PHP server.
	 * @param isPost If the request is a Post or not.
	 */
	void fastcgi_params(int fd, unsigned short request_id,char *file_name, int port, int isPost);
	
	/**
	 * @brief Try adding a value from the syntaxic tree if the value exist, do noting if not.
	 * 
	 * @param h The pointer to the current header.
	 * @param name The name of the param.
	 * @param value The name of the tree node.
	 */
	void try_add_params(FCGI_Header *h, char *name, char *value);
	
	/**
	 * @brief Add the script_filename param.
	 * 
	 * @param h The pointer to the current header.
	 * @param file_name The path of the target file.
	 * @param port The port of the server.
	 */
	void add_params_script(FCGI_Header *h, char *file_name, int port);
	
	/**
	 * @brief Add the query param.
	 * 
	 * @param h The pointer to the current header.
	 */
	void add_params_query(FCGI_Header *h);
	

	/**
	 * @brief Handle answer from the server.
	 * 
	 * @param fd The id of the current tube.
	 * @param request_id A number use to tell which request we are in.
	 */
	void fastcgi_answer(int fd, unsigned short request_id );
	
	/**
	 * @brief Read a Header from the socket.
	 * 
	 * @param fd  The id of the current tube.
	 * @return The pointer to the received message.
	 */
	FCGI_Header *readData_socket(int fd);
	
	/**
	 * @brief Create a char * containing all the data of all response from the server.
	 * 
	 * @param list The pointer to the linked list of all the answers given by the php server.
	 * @param len The number of header in the linked list.
	 * @return A dynamically allocated char* with all the answers in it.
	 */
	char *fastcgi_concat_stdout(Fastcgi_stdout_linked *list, int * len);
	
	/**
	 * @brief Read all the responses and file data according to the content.
	 * 
	 * @param _data The pointer to the char* storing all the answers givent by the php server.
	 * @param _len The length of this char *.
	 */
	void handle_fastcgi_data(char * _data, int _len);

	/**
	 * @brief Add an Header to the linked list.
	 * 
	 * @param list The pointer to the char* storing all the answers givent by the php server.
	 * @param header A header we want to add to the list.
	 */
	void add_stdout_list(Fastcgi_stdout_linked **list, FCGI_Header *header);
	
	/**
	 * @brief Purge the linked list from memory.
	 * 
	 * @param list The pointer to the char* storing all the answers givent by the php server.
	 */
	void purge_stdout_list(Fastcgi_stdout_linked **list);
	
	/**
	 * @brief Get Header at the position i in the linked list.
	 * 
	 * @param list The pointer to the char* storing all the answers givent by the php server.
	 * @param i 
	 * @return The header at the i'position.
	 */
	FCGI_Header *get_stdout_list(Fastcgi_stdout_linked *list, int i);

	/**
	 * @brief Get the body for the HTTP response.
	 * 
	 * @param len Store the length of the body in this variable.
	 * @return The body of the HTTP reponse given by php.
	 */
	char * fastcgi_get_body(int *len);

	/**
	 * @brief Give the type of the file given by php.
	 * 
	 * @return The type of the file given by php.
	 */
	char * fastcgi_get_type();

	/**
	 * @brief Get the current HTTP error code.
	 * 
	 * @return The HTTP error code given by php.
	 */
	int fastcgi_get_error();

#endif






