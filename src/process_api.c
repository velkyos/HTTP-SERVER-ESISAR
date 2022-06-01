/**
 * @file process_api.c
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

#include "process_api.h"
#include "syntax_api.h"
#include "answer_api.h"
#include "fastcgi_api.h"

/* Constants */

#define C_200 "HTTP/1.1 200 OK"
#define C_201 "HTTP/1.1 201 Created"
#define C_400 "HTTP/1.1 400 Bad Request"
#define C_404 "HTTP/1.1 404 Not Found"
#define C_501 "HTTP/1.1 501 Not Implemented"
#define C_GET 1
#define C_POST 2
#define C_HEAD 3

/* Declaration */

/**
 * @brief A simple structure to store information about a file.
 *
 */
typedef struct str_fileData{
	char *name;
	char *data;
	int len;
	int status;
} FileData;
/**
 * @brief Function who process HEAD and GET methods.
 * @see Answer_list
 *
 * @param isGet 'Boolean' to tell if it's a GET or an HEAD method.
 * @return Return an linked list with all the parts of the answer.
 */
Answer_list *process_method(int isGet);

/**
 * @brief Function who process Bad request.
 * @see Answer_list
 *
 * @param error_code The char * of the status line.
 * @return Return an linked list with all the parts of the answer.
 */

Answer_list *process_errors(char *error_code);
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
/**
 * @brief Retrieve file data.
 * @see get_file_name()
 *
 * @return Return the Name, length and the data of the file.
 */
FileData *get_file_data(int isPost);
/**
 * @brief Write data to file.
 * @see get_file_name()
 *
 * @return Return 2 if file as been created, 0 if the file as been replaced and 1 for errors
 */
FileData * push_file_data(char *name);
/**
 * @brief Use magiclib to get the type of the file the client want to access.
 *
 * @param file_name
 * @return Return the type in the HTTP format.
 */
char *get_content_type(char *file_name);

char *get_complete_path();
/**
 * @brief A utility function who job is to copy the _value into a new allocated char* and insert the resulting one into the answer_list.
 *
 * @param _value The text you want to copy.
 * @param answer The adress of the linked list.
 */
void copy_to_answer(char *_value, int len , Answer_list **answer);
/** @brief Get the http_version used by the client.*/
void get_http_version();
/** @brief Generate the age header (Value of 0 because no cache)*/
void generate_age_header(Answer_list **answer);
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

void get_current_website();

/* Definition */

 /** The config file */
Config_server *config = NULL;
 /** Current connection_status */
int connection_status = PRO_UNKNOWN;
 /** Current HTTP_version of the client */
int current_version = 0;
 /** Is the current request a request to the php server */
int is_php = 0;
Website *c_site = NULL;

char *process_request(Config_server *_config, int *anwser_len){
	config = _config;
	connection_status = PRO_CLOSE;
    Answer_list *answer = NULL;
	is_php = 0;

	if ( getRootTree() == NULL){ //If syntax or semantic is not valid
		answer = process_errors(C_400);
	}
	else{ //If syntax and semantic is valid
		get_http_version();
		get_current_website();

		_Token *method = searchTree( NULL , "method");

		if ( strncmp( getElementValue(method->node, NULL), "GET",3) == 0)
		{
			answer = process_method( C_GET );
			
		}
		else if ( strncmp( getElementValue(method->node, NULL), "HEAD" ,4)  == 0)
		{
			answer = process_method( C_HEAD );
		}
		else if (strncmp( getElementValue(method->node, NULL), "POST" ,4)  == 0)
		{
			answer = process_method( C_POST);
			//fastcgi_request("test.php",10,9000, 1);
		}
		else { //Not implemented method
			answer = process_errors(C_501);
		}
		purgeElement(&method);
	}

	

    char * answer_text = concat_answers(answer, anwser_len);
	purge_answer(&answer);

	return answer_text;
}

Answer_list *process_errors(char *error_code){
	Answer_list *answer = NULL;

	//Add the status line
	add_node_answer( &answer, A_TAG_STATUS, error_code, strlen(error_code), !A_CANFREE);

	//Add headers
	generate_date_header(&answer);
	generate_content_length_header(&answer, NULL);
	generate_server_header(&answer);

	//Add body
	generate_body(&answer, NULL);

	return answer;
}

Answer_list *process_method(int method){
	Answer_list *answer = NULL;
	FileData *file = get_file_data( method == C_POST );

	if(file->data == NULL) return process_errors(C_404);

	generate_status(&answer, file);

	generate_header_fields(&answer, file);

	if ( method == C_GET || method == C_POST ) generate_body(&answer, file);
	else generate_body(&answer, NULL);

	if ( file != NULL){
		
		if ( file->name != NULL) free(file->name);
		free(file);
	}

	return answer;
}

FileData *get_file_data(int isPost){

	char *name = get_complete_path();

	/*TODO : DETECT PHP */
	is_php = strstr( name, ".php") != NULL;
	if (is_php) fastcgi_request( name , 1 , config->phpport , c_site, isPost);

	if( isPost ) return push_file_data(name);

	FileData *file = malloc(sizeof(FileData));
	file->name = name;
	file->data = NULL;
	file->status = 404;
	file->len = 0;

	if( is_php ){
		//On free le nom ce qui permet de déterminer si c'est du php ou non
		if( file->name != NULL ) free(file->name);
		file->name = NULL;
		file->data = fastcgi_get_body(&file->len);
		file->status = 200;
		return file;
	}

	if(file->name == NULL) return file;

	file->data = read_file( file->name, &file->len);

	if(file->data == NULL) return file;

	file->status = 200;
	return file;
}

FileData * push_file_data(char *name){
	FileData *file = malloc(sizeof(FileData));
	file->name = name;
	file->data = NULL;
	file->status = 404;
	file->len = 0;

	if( is_php ){
		//On free le nom ce qui permet de déterminer si c'est du php ou non
		if( file->name != NULL ) free(file->name);
		file->name = NULL;
		file->data = fastcgi_get_body(&file->len);
		file->status = 200;
		return file;
	}

	_Token *method = searchTree( NULL , "message-body");

	if( method != NULL){
		file->data = getElementValue(method->node, NULL);
		file->len = strlen(getElementValue(method->node, NULL));
		purgeElement(&method);
	}

	if(file->name == NULL || file->data == NULL) return file;

	int result = write_file( file->name,file->data, file->len);

	if( result == 0) file->status = 200;
	else if( result == 2) file->status = 201;

	return file;
}

char *get_complete_path(){
	if (c_site == NULL) return NULL; //We do nothing if the host is wrong or if there is not website in the config file.

	char *_name = get_file_name();
	
	//printf("Nom fichier : %s\n", _name);

	int len = strlen(_name) + c_site->root_len + 1;
	char *name = malloc(len);
	memset(name, '\0', len);
	strcpy(name, c_site->root);
	strcat(name, _name);

	free(_name);
	return name;
}

char *get_file_name(){

	if (c_site == NULL) return NULL; //We do nothing if the host is wrong or if there is not website in the config file.

	_Token *target = searchTree(NULL , "absolute-path");
	int _len = 0;
	char *_name = getElementValue( target->node, &_len);
	_name = percent_encoding(_name, _len);


	char *name = NULL;
	if( _len == 1){ //If the target is /, we get the index file in the config
		int len = _len + c_site->index_len + 1;
		name = malloc(len);
		if (name){
			memset(name, '\0', len);
			strncpy(name, _name, len);
			strcat(name, c_site->index);
		}
	}
	else{
		int len = _len + 1;
		name = malloc(len);
		if (name){
			memset(name, '\0', len);
			strncpy(name, _name, len);
		}
	}

	free(_name);
	purgeElement(&target);

	return name;
}

int get_connection_status(){
	return connection_status;
}

void copy_to_answer(char *_value, int len , Answer_list **answer){
	if( len == -1 ) len = strlen(_value);
	char *value = malloc(len + 1);

	memset(value, '\0', len + 1); //We fill all with 0

	if(value){
		memcpy(value, _value, len ); //Copy the _value
		add_node_answer( answer, A_TAG_HEADER, value, len, A_CANFREE);
	}
}

char * get_content_type(char *file_name){
	const char *magic_full = NULL;
	char * charset = NULL;
	char * res = NULL;
	magic_t magic_cookie;
	int size;

	magic_cookie = magic_open(MAGIC_MIME); //Creation magic cookie
	if(magic_cookie == NULL){
		perror("Erreur magic_open");
		return NULL;
	}

	if(magic_load(magic_cookie,NULL) != 0){ //Chargement
		perror("erreur magic_load");
		return NULL;
	}
	magic_full = magic_file(magic_cookie,file_name); //Retour la description du namefile

	if(strstr(magic_full,"text/plain;")!=NULL){ //Cas css et js qui n'est pas prix en compte pat libmagic
		charset = strstr(magic_full,"charset="); //recherche charset

		if(strstr(file_name,".css")!=NULL){ //cherche un .css
			size = strlen(charset) + 11;
			res = malloc(size);
			if (res) sprintf(res, "text/css; %s", charset);

		}else if(strstr(file_name,".js")!=NULL){ //cherche un .js
			size = strlen(charset) + 18;
			res = malloc(size);
			if (res) sprintf(res, "text/javascript; %s", charset);

		}
	}else{
		size = strlen(magic_full);
		res = malloc(size + 1);
		if(res) {
			memcpy(res, magic_full ,size + 1); //Convertion const char * en char *
		}
	}
	magic_close(magic_cookie);
	return res;
}

void get_http_version(){
	_Token *version = searchTree(NULL, "HTTP-version");
	int version_len = 0;
	char *version_val = getElementValue( version->node, &version_len);

	current_version = (compare_string(version_val,"HTTP/1.1")) ? 1 : 0;
	purgeElement(&version);
}

void get_current_website(){
	if( current_version == 1){ //We check the host header only in HTTP/1.1
		_Token *host = searchTree(NULL, "host");
		int host_len = 0;
		char *host_val = getElementValue( host->node, &host_len);

		c_site = find_website(config, host_val, host_len);

		purgeElement(&host);
	} else {
		c_site = config->websites; //If HTTP/1.0 we put the first website in the config file
	}
}

/*
Generation of response
*/

void generate_status(Answer_list **answer, FileData *file){
	int status = is_php ? fastcgi_get_error() : file->status;

	switch (status)
	{
	case 200:
		add_node_answer( answer, A_TAG_STATUS, C_200, strlen(C_200), !A_CANFREE);
	break;
	case 404:
		add_node_answer( answer, A_TAG_STATUS, C_404, strlen(C_404), !A_CANFREE);
		break;
	case 201:
		add_node_answer( answer, A_TAG_STATUS, C_201 , strlen(C_201), !A_CANFREE);
		break;
	default:
		char s[50] = "";
		sprintf(s, "HTTP/1.1 %d", status);
		add_node_answer( answer, A_TAG_STATUS, s , strlen(s), !A_CANFREE);
		break;
	}
}

void generate_header_fields(Answer_list **answer, FileData *file){
	generate_content_length_header(answer, file);
	generate_server_header(answer);
	generate_connection_header(answer);
	generate_date_header(answer);
	generate_Allow_header(answer);
	if(file->data != NULL){
		generate_content_type_header(answer, file);
	}
}

void generate_body(Answer_list **answer, FileData *file){
	int canFree = (file->name  == NULL) ? A_CANFREE : !A_CANFREE;
	if( file->data != NULL){
		add_node_answer( answer, A_TAG_BODY, file->data, file->len, canFree);
	}
	else{
		add_node_answer( answer, A_TAG_BODY, "", 0, !A_CANFREE);
	}
}

void generate_date_header(Answer_list **answer){
	time_t t;
	time(&t);

	char *value_t = gmt_time(&t);

	char value[50] = "";
	sprintf(value, "Date: %s", value_t);

	copy_to_answer(value, -1, answer);

	free(value_t);
}

void generate_Allow_header(Answer_list **answer){
	add_node_answer( answer, A_TAG_HEADER, "Allow: GET, HEAD, POST", 16, !A_CANFREE);
}

void generate_content_length_header(Answer_list **answer, FileData *file){
	char value[50] = "";

	if ( file != NULL) sprintf(value, "Content-Length: %d", file->len);
	else sprintf(value, "Content-Length: %d", 0);

	copy_to_answer(value, -1, answer);
}

void generate_content_type_header(Answer_list **answer, FileData *file){
	char *type = NULL;
	char value[80] = "";

	if( is_php ) type = fastcgi_get_type();
	else type = get_content_type(file->name);

	sprintf(value, "Content-Type: %s", type);
	free(type);
	copy_to_answer(value, -1, answer);
}

void generate_server_header(Answer_list **answer){
	add_node_answer( answer, A_TAG_HEADER, "Server: Esisar Groupe 9", 24, !A_CANFREE);
}

void generate_keep_alive_header(Answer_list **answer){
	char value[50] = "";
	sprintf(value, "Keep-Alive: timeout=%d, max=%d", config->keepTimeOut, config->keepMax);

	copy_to_answer(value,-1,  answer);
}

void generate_connection_header(Answer_list **answer){
	char value[50] = "";

	_Token *tok = searchTree(NULL, "Connection");
	if( tok != NULL){
		int tok_len;
		char *tok_val = getElementValue(tok->node, &tok_len);

		if(strncmp("keep-alive", tok_val, tok_len) == 0){
			generate_keep_alive_header(answer);
			connection_status = PRO_KEEP_ALIVE;
		}else if (strncmp("close", tok_val, tok_len) == 0){
			connection_status = PRO_CLOSE;
		}

		sprintf(value, "Connection: %.*s", tok_len, tok_val);

		copy_to_answer(value, -1, answer);
	}

	purgeElement(&tok);
}
