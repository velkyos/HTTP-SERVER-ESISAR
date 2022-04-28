/**
 * @file process_api.c
 * @author ROBERT Benjamin
 * @brief Process of an HTTP request.
 * @version 0.1
 * @date 2022-04-8
 * 
 * @copyright Copyright (c) 2022
 * 
 */
/* System Includes */

#include <string.h>
#include <time.h>
#include <stdlib.h>

/* User Includes */

#include "process_api.h"
#include "syntax_api.h"
#include "answer_api.h"

/* Constants */

#define C_200 "HTTP/1.1 200 OK"
#define C_404 "HTTP/1.1 404 Not Found"
#define C_501 "HTTP/1.1 501 Not Implemented"

/* Declaration */

typedef struct str_fileData{
	char *name;
	char *data;
	int len;
} FileData;

Answer_list *process_head(derivation_tree *request, int isGet);

void generate_status(derivation_tree *request, Answer_list **answer, FileData *file);
void generate_header_fields(derivation_tree *request, Answer_list **answer, FileData *file);
void generate_body(Answer_list **answer, FileData *file);

FileData *get_file_data(derivation_tree *request);
char *get_file_name(derivation_tree *request);

void generate_age_header(Answer_list **answer);
void generate_date_header(Answer_list **answer);
void generate_Allow_header(Answer_list **answer);
void generate_content_length_header(Answer_list **answer, FileData *file);
void generate_content_type_header(Answer_list **answer, FileData *file);
void generate_server_header(Answer_list **purge_answer);
void generate_keep_alive_header(derivation_tree *request, Answer_list **answer);
void generate_connection_header(derivation_tree *request, Answer_list **answer);

/* Definition */

Config_server *config = NULL;

char *process_request(derivation_tree *request, Config_server *_config, int *anwser_len){
	config = _config;
    Answer_list *answer = NULL;

    _Token *method = searchTree( request, "method");

    if ( strcasecmp( getElementValue(method->node, NULL), "GET" ))
    {
        answer = process_head(request, 1);
    }
	else if ( strcasecmp( getElementValue(method->node, NULL), "HEAD" ))
    {
        answer = process_head(request, 0);
    }
    else {
		add_node_answer( &answer, UTI_STATUS, C_501, strlen(C_501), 0);
    }

    char * answer_text = concat_answer(answer, anwser_len);

	purge_answer(&answer);

	return answer_text;
}

Answer_list *process_head(derivation_tree *request, int isGet){
	Answer_list *answer = NULL;
	FileData *file = get_file_data(request);
	
	generate_status(request, &answer, file);

	generate_header_fields(request, &answer, file);

	if ( isGet == 1) generate_body(&answer, file);

	if ( file != NULL){
		free(file->name);
		free(file);
	}

	return answer;
}

FileData *get_file_data(derivation_tree *request){
	FileData *file = malloc(sizeof(FileData));
	file->name = NULL;
	file->data = NULL;

	file->name = get_file_name(request);

	if(file->name == NULL) {
		free(file);
		return NULL;
	}

	file->data = read_file( file->name, &file->len);

	if(file->data == NULL) {
		free(file->name);
		free(file);
		return NULL;
	}
	return file;
}

char *get_file_name(derivation_tree *request){
	
	_Token *host = searchTree(request, "host");
	int host_len = 0;
	char *host_val = getElementValue( host->node, &host_len);
	

	_Token *target = searchTree(request, "absolute-path");
	int target_len = 0;
	char *target_val = getElementValue( target->node, &target_len);
	
	Website *site = find_website(config, host_val, host_len);
	if (site == NULL) return NULL;

	char *name = NULL;
	if( target_len == 1){
		name = malloc((target_len + strlen(site->root) + strlen(site->index) + 1) * sizeof(char));
		strcpy(name, site->root);
		strncat(name, target_val, target_len);
		strcat(name, site->index);
	}
	else{
		name = malloc( target_len + strlen(site->root) + 1);
		strcpy(name, site->root);
		strncat(name, target_val, target_len);
	}

	purgeElement(&host);
	purgeElement(&target);

	return name;
}

void generate_status(derivation_tree *request, Answer_list **answer, FileData *file){
	if(file == NULL) {
		add_node_answer( answer, UTI_STATUS, C_404, strlen(C_404), 0);
	}
	else{
		add_node_answer( answer, UTI_STATUS, C_200, strlen(C_200), 0);
	}
}

void generate_header_fields(derivation_tree *request, Answer_list **answer, FileData *file){
	if(file != NULL){
		generate_content_length_header(answer, file);
		generate_content_type_header(answer, file);
	}
	generate_age_header(answer);
	generate_server_header(answer);
	generate_connection_header(request, answer);
	generate_date_header(answer);
	generate_Allow_header(answer);
}

void generate_body(Answer_list **answer, FileData *file){
	if( file != NULL) {
		add_node_answer( answer, UTI_BODY, file->data, file->len, 0);
	}
	else{
		add_node_answer( answer, UTI_BODY, "", 0, 0);
	}
}

void generate_age_header(Answer_list **answer){
	add_node_answer( answer, UTI_HEADER, "Age: 0", 6, 0);
}
void generate_date_header(Answer_list **answer){
	time_t t;
	time(&t);

	char *value_t = gmt_time(&t);
	char value[50] = "";
	sprintf(value, "Date: %s", value_t);
	char *rvalue = malloc(strlen(value) * sizeof(char));

	strcpy(rvalue, value);
	add_node_answer( answer, UTI_HEADER, rvalue, strlen(rvalue), UTI_CANFREE);

	free(value_t);
}
void generate_Allow_header(Answer_list **answer){
	add_node_answer( answer, UTI_HEADER, "Allow: GET, HEAD", 16, 0);
}
void generate_content_length_header(Answer_list **answer, FileData *file){
	char value[50] = "";
	sprintf(value, "Content-Length: %d", file->len);
	char *rvalue = malloc(strlen(value) * sizeof(char));

	strcpy(rvalue, value);
	add_node_answer( answer, UTI_HEADER, rvalue, strlen(rvalue), UTI_CANFREE);
}
void generate_content_type_header(Answer_list **answer, FileData *file){
	add_node_answer( answer, UTI_HEADER, "Content-Type: text/html; charset=UTF-8", 38, 0);
}	
void generate_server_header(Answer_list **answer){
	add_node_answer( answer, UTI_HEADER, "Server: Esisar Groupe 10", 24, 0);
}
void generate_keep_alive_header(derivation_tree *request, Answer_list **answer){
	char value[50] = "";
	sprintf(value, "Keep-Alive: timeout=%d, max=%d", config->keepTimeOut, config->keepMax);
	char *rvalue = malloc(strlen(value) * sizeof(char));

	strcpy(rvalue, value);
	add_node_answer( answer, UTI_HEADER, rvalue, strlen(rvalue), UTI_CANFREE);
}
void generate_connection_header(derivation_tree *request, Answer_list **answer){
	char value[50] = "";

	_Token *tok = searchTree(request, "Connection");
	if( tok != NULL){
		int tok_len;
		char *tok_val = getElementValue(tok->node, &tok_len);

		if(strncmp("keep-alive", tok_val, tok_len) == 0){
			generate_keep_alive_header(request, answer);
			/* Handle Keep-alive */
		}else if (strncmp("close", tok_val, tok_len) == 0){
			/* Handle close */
		}

		sprintf(value, "Connection: %.*s", tok_len, tok_val);
		char *rvalue = malloc(strlen(value) * sizeof(char));

		strcpy(rvalue, value);
		add_node_answer( answer, UTI_HEADER, rvalue, strlen(rvalue), UTI_CANFREE);
	}
}