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
#include "utils.h"

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

derivation_tree *process_head(derivation_tree *request, int isGet);

void generate_header_fields(derivation_tree *request, derivation_tree *answer, FileData *file);
void generate_body(derivation_tree *request, derivation_tree *answer, FileData *file);

FileData *get_file_data(derivation_tree *request);
char *get_file_name(derivation_tree *request);

/* Definition */

Config_server *config = NULL;

derivation_tree *process_request(derivation_tree *request, Config_server *_config){
	config = _config;
    derivation_tree *ans_tree = NULL;

    _Token *method = searchTree( request, "method");

    if ( strcasecmp( getElementValue(method->node, NULL), "GET" ))
    {
        ans_tree = process_head(request, 1);
    }
	else if ( strcasecmp( getElementValue(method->node, NULL), "HEAD" ))
    {
        ans_tree = process_head(request, 0);
    }
    else {
		ans_tree = create_tree_node("HTTP-message",NULL,0,0);
        add_child_to_list( &ans_tree->children, create_tree_node("start-line",C_501, strlen(C_501), 1));
    }

    return ans_tree;
}

derivation_tree *process_head(derivation_tree *request, int isGet){
	derivation_tree *answer = create_tree_node("HTTP-message",NULL,0,0);
	FileData *file = get_file_data(request);
	
	generate_header_fields(request, answer, file);

	if ( isGet == 1) generate_body(request, answer, file);

	if ( file != NULL){
		free(file->data);
		free(file->name);
		free(file);
	}

	return answer;
}

FileData *get_file_data(derivation_tree *request){
	FileData *file = malloc(sizeof(FileData));

	file->name = get_file_name(request);

	if(file->name == NULL) return NULL;

	file->data = read_file( file->name, &file->len);

	if(file->data == NULL) return NULL;

	return file;
}

char *get_file_name(derivation_tree *request){
	int host_len;
	char *host = getElementValue( searchTree(request, "host")->node, &host_len);
	Website *site = find_website(config, host, host_len);

	int target_len;
	char *target = getElementValue( searchTree(request, "absolute-path")->node, &target_len );
	
	if (site == NULL) return NULL;

	char *name = NULL;
	if( target_len == 1){
		name = malloc( target_len + strlen(site->root) + strlen(site->index));

		strcpy(name, site->root);
		strncat(name, target, target_len);
		strcat(name, site->index);
	}
	else{
		name = malloc( target_len + strlen(site->root));

		strcpy(name, site->root);
		strncat(name, target, target_len);
	}
	return name;
}

void generate_header_fields(derivation_tree *request, derivation_tree *answer, FileData *file){
	if(file == NULL) {
		add_child_to_list( &answer->children, create_tree_node("start-line",C_404, strlen(C_404), 1));
	} 
}

void generate_body(derivation_tree *request, derivation_tree *answer, FileData *file){

}