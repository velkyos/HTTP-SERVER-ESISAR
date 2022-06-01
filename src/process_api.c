#include "process_api.h"

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
		answer = process_errors(PRO_C_400);
	}
	else{ //If syntax and semantic is valid
		get_http_version();
		get_current_website();

		_Token *method = searchTree( NULL , "method");

		if ( strncmp( getElementValue(method->node, NULL), "GET",3) == 0)
		{
			answer = process_method( PRO_GET );
		}
		else if ( strncmp( getElementValue(method->node, NULL), "HEAD" ,4)  == 0)
		{
			answer = process_method( PRO_HEAD );
		}
		else if (strncmp( getElementValue(method->node, NULL), "POST" ,4)  == 0)
		{
			answer = process_method( PRO_POST);
		}
		else { //Not implemented method
			answer = process_errors(PRO_C_501);
		}
		purgeElement(&method);
	}

	

    char * answer_text = concat_answers(answer, anwser_len);

	purge_answer(&answer);

	return answer_text;
}

Answer_list *process_method(int method){
	Answer_list *answer = NULL;
	FileData *file = get_file_data( method == PRO_POST );

	if( file->status == 404) get_404_page(file);

	//printf("%s\n",file->name);
	//printf("%s\n",file->data);

	generate_status(&answer, file);

	generate_header_fields(&answer, file);

	if ( method == PRO_GET || method == PRO_POST ) generate_body(&answer, file);
	else generate_body(&answer, NULL);

	if ( file != NULL){
		
		if ( file->name != NULL) free(file->name);
		free(file);
	}

	return answer;
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

void copy_to_answer(char *_value, int len , Answer_list **answer){
	if( len == -1 ) len = strlen(_value);
	char *value = malloc(len + 1);

	memset(value, '\0', len + 1); //We fill all with 0

	if(value){
		memcpy(value, _value, len ); //Copy the _value
		add_node_answer( answer, A_TAG_HEADER, value, len, A_CANFREE);
	}
}

/*
Getting Information
*/

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

FileData *get_file_data(int isPost){

	char *name = get_complete_path(NULL);

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

	if( is_php ) file->type = fastcgi_get_type();
	else file->type = get_content_type(file->name);

	is_acceptable(file);

	return file;
}

char *get_complete_path(char *_filename){
	if (c_site == NULL) return NULL; //We do nothing if the host is wrong or if there is not website in the config file.

	char *_name = get_file_name(_filename);
	
	//printf("Nom fichier : %s : %d\n", _name, strlen(_name));

	int len = strlen(_name) + strlen(c_site->root) + 1;
	char *name = malloc(len);
	memset(name, '\0', len);
	strcpy(name, c_site->root);
	strcat(name, _name);

	free(_name);

	return name;
}

char *get_file_name(char *_name){

	if (c_site == NULL) return NULL; //We do nothing if the host is wrong or if there is not website in the config file.

	int _len = 0;
	if(!_name){
		_Token *target = searchTree(NULL , "absolute-path");
		_name = getElementValue( target->node, &_len);
		purgeElement(&target);
	}
	else{
		_len = strlen(_name);
	}

	_name = percent_encoding(_name, _len);


	char *name = NULL;
	if( _len == 1){ //If the target is /, we get the index file in the config
		int len = _len + strlen(c_site->index) + 1;
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

	return name;
}

void get_404_page(FileData *file){

	file->type = malloc(26);
	memset(file->type, '\0', file->len + 1);
	memcpy(file->type,"text/html; charset=utf-8", 25);

	if( c_site->page_404 == NULL){
		char *name = get_file_name(NULL);
		file->len = 242 + strlen(name);
		file->data = malloc( file->len + 1);
		memset(file->data, '\0', file->len + 1);
		sprintf(file->data, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n<p>The requested URL %s was not found on this server.</p>\n<hr>\n<address>Server HTTP Groupe 9</address>\n</body></html>",name);
		free(name);
	}
	else{
		int len = strlen(c_site->page_404);
		char *temp = malloc(len + 2);
		memset(temp, '\0', len + 2);
		*temp = '/';
		strcat(temp, c_site->page_404);
	}
}

char *get_content_type(char *file_name){
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

void is_acceptable(FileData *file){
	char *accept = get_header_val("Accept");

	char *semilicon = strchr(file->type, ';');
	int slen = (int)(semilicon - file->type); // len from the start to ;

	char *mime_subtype = malloc( slen + 1); // len + \0
	memset(mime_subtype,'\0', slen + 1);
	memcpy(mime_subtype, file->type, slen);

	char *slash = strchr(file->type, '/');
	int mlen = (int)(slash - file->type) + 1; // len from the start to /

	char *mime_type = malloc( mlen + 2); // len + *\0
	memset(mime_type,'\0', mlen + 2);
	memcpy(mime_type, file->type, mlen);
	strcat(mime_type,"*");

	//printf("Type : %s - %s\n",mime_type,mime_subtype);

	if( strstr(accept, PRO_ACCEPT_ALL) == NULL)
	{	
		if( strstr(accept, mime_subtype) == NULL )
		{
			if( strstr(accept, file->type) == NULL )
			{
				file->status = 406;
			}
		}
	}
	
	free(mime_subtype);
	free(mime_type);
	free(accept);
}

int get_connection_status(){
	return connection_status;
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
		add_node_answer( answer, A_TAG_STATUS, PRO_C_200, strlen(PRO_C_200), !A_CANFREE);
	break;
	case 404:
		add_node_answer( answer, A_TAG_STATUS, PRO_C_404, strlen(PRO_C_404), !A_CANFREE);
		break;
	case 201:
		add_node_answer( answer, A_TAG_STATUS, PRO_C_201 , strlen(PRO_C_201), !A_CANFREE);
		break;
	case 406:
		add_node_answer( answer, A_TAG_STATUS, PRO_C_406 , strlen(PRO_C_406), !A_CANFREE);
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
	add_node_answer( answer, A_TAG_HEADER, "Allow: GET, HEAD, POST", 23, !A_CANFREE);
}

void generate_content_length_header(Answer_list **answer, FileData *file){
	char value[50] = "";

	if ( file->data != NULL) sprintf(value, "Content-Length: %d", file->len);
	else sprintf(value, "Content-Length: %d", 0);

	copy_to_answer(value, -1, answer);
}

void generate_content_type_header(Answer_list **answer, FileData *file){
	char *type = file->type;

	if( type ) {
		type = malloc(11);
		memset(type,'\0',11);
		memcpy(type,"text/html",10);
	}

	char value[80] = "";

	sprintf(value, "Content-Type: %s", type);
	free(type);
	copy_to_answer(value, -1, answer);
}

void generate_server_header(Answer_list **answer){
	add_node_answer( answer, A_TAG_HEADER, "Server: Esisar Groupe 9", 24, !A_CANFREE);
}

void generate_keep_alive_header(Answer_list **answer){
	add_node_answer( answer, A_TAG_HEADER, "Keep-Alive: timeout=5, max=15", 30, !A_CANFREE);
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
