#ifndef PROCESS_API_H
    #define PROCESS_API_H

	/**
	 * @file process_api.h
	 * @author ROBERT Benjamin & PEDER Leo & MANDON Anaël
	 * @brief Process of an HTTP request.
	 * @version 0.6
	 * @date 2022-04-8
	 *
	 *
	 */
	/* System Includes */

	#include <string.h>
	#include <time.h>
	#include <stdlib.h>
	#include <magic.h>

	/* User Includes */

	#include "fastcgi_api.h"
	#include "syntax_api.h"
	#include "answer_api.h"
	#include "config.h"

	/* Constants */

	#define PRO_C_200 "HTTP/1.1 200 OK"
	#define PRO_C_201 "HTTP/1.1 201 Created"
	#define PRO_C_400 "HTTP/1.1 400 Bad Request"
	#define PRO_C_404 "HTTP/1.1 404 Not Found"
	#define PRO_C_406 "HTTP/1.1 406 Not Acceptable"
	#define PRO_C_501 "HTTP/1.1 501 Not Implemented"
	#define PRO_GET 1
	#define PRO_POST 2
	#define PRO_HEAD 3
	#define PRO_ACCEPT_ALL "*/*"
	#define PRO_UNKNOWN 0
	#define PRO_CLOSE 1
	#define PRO_KEEP_ALIVE 2

	/**
	 * @brief A simple structure to store information about a file.
	 */
	typedef struct str_fileData{
		char *name;
		char *data;
		char *type;
		int len;
		int status;
	} FileData;


	/* Primary Function */

	/**
	 * @brief Main function called from the main function to process an HTTP request.
	 * 
	 * @param _config The address of the current configuration.
	 * @param anwser_len The pointer where the length of the dynamically allocated char*.
	 * @return A dynamically allocated char* storing all the answer for the HTTP client.
	 */
	char *process_request(Config_server *_config, int *anwser_len);

	/**
	 * @brief Function who process methods.
	 * @see Answer_list 
	 * 
	 * @param method Value of : PRO_GET | PRO_POST | PRO_HEAD
	 * @return Return an linked list with all the parts of the answer.
	 */
	Answer_list *process_method(int method);

	/**
	 * @brief Function who process only simple Answer based on an error.
	 * @see Answer_list
	 *
	 * @param error_code The char * of the status line.
	 * @return Return an linked list with all the parts of the answer.
	 */

	Answer_list *process_errors(char *error_code);

/**
	 * @brief A utility function who job is to copy the _value into a new allocated char* and insert the resulting one into the answer_list.
	 *
	 * @param _value The text you want to copy.
	 * @param answer The adress of the linked list.
	 */
	void copy_to_answer(char *_value, int len , Answer_list **answer);

	/* Getting Information function */

	/**
	 * @brief Write data to file.
	 * @see get_file_name()
	 *
	 * @return Return 2 if file as been created, 0 if the file as been replaced and 1 for errors
	 */
	FileData * push_file_data(char *name);

	/**
	 * @brief Retrieve file data.
	 * @see get_file_name()
	 *
	 * @return Return the Name, length and the data of the file.
	 */
	FileData *get_file_data(int isPost);

	/**
	 * @brief Get the complete path of the target file.
	 * @see Config_server
	 * @see Website
	 * 
	 * @param _filename The name of the file, we want to get the complete_path, leave NULL for the target file.
	 * @return A dynamically allocated char*.
	 */
	char *get_complete_path(char *_filename);

	/**
	 * @brief Getting the file name of the targeted file, after percent encoding and dot dot removal.
	 * @see Config_server
	 * @see Website
	 *
	 * @return A dynamically allocated char*.
	 */
	char *get_file_name(char *_name);

	/**
	 * @brief Get the corresponding 404 page. A defined one or a generated one.
	 * 
	 * @param file
	 */
	void get_404_page(FileData *file);

	/**
	 * @brief Use magiclib to get the type of the file the client want to access.
	 *
	 * @param file_name
	 * @return Return the type in the HTTP format.
	 */
	char *get_content_type(char *file_name);

	/**
	 * @brief Replace the status code by 406 if the file type is not in the ACCEPT header.
	 * 
	 * @param file
	 */
	void is_acceptable(FileData *file);

	/**
	 * @brief Get the connection status.
	 * 
	 * @return The connection status, PRO_CLOSE | PRO_KEEP_ALIVE
	 */
	int get_connection_status();

	/** @brief Get the http_version used by the client.*/
	void get_http_version();

	/**
	 * @brief Get the current website.
	 * 
	 */
	void get_current_website();

	/* Generation of the answer */

	/**
	 * @brief Generate the status and copy it into the answer_list.
	 *
	 * @param answer The adress of the linked list.
	 * @param file The adress of the information about the file.
	 */
	void generate_status(Answer_list **answer, FileData *file);
	/**
	 * @brief Generate header fields and copy it into the answer_list.
	 *
	 * @param answer The adress of the linked list.
	 * @param file The adress of the information about the file.
	 */
	void generate_header_fields(Answer_list **answer, FileData *file);
	/**
	 * @brief Generate the body and copy it into the answer_list.
	 *
	 * @param answer The adress of the linked list.
	 * @param file The adress of the information about the file.
	 */
	void generate_body(Answer_list **answer, FileData *file);

	/** @brief Generate the date header with the correct format.*/
	void generate_date_header(Answer_list **answer);
	/** @brief Generate the allow header with GET and HEAD*/
	void generate_Allow_header(Answer_list **answer);
	/** @brief Generate the content length header.*/
	void generate_content_length_header(Answer_list **answer, FileData *file);
	/**
	 * @brief Generate the content type header.
	 * @see get_content_type()
	 * */
	void generate_content_type_header(Answer_list **answer, FileData *file);
	/** @brief Generate the server header.*/
	void generate_server_header(Answer_list **purge_answer);
	/** @brief Generate the Keep Alive header.*/
	void generate_keep_alive_header(Answer_list **answer);
	/** @brief Generate the connection header header.*/
	void generate_connection_header(Answer_list **answer);


#endif


