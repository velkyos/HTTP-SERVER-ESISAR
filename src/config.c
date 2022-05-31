#include "config.h"

Config_server *get_config(char *file_name){
	char *file = read_file(file_name, NULL);

    if (file == NULL) return NULL;

    Config_server *config = malloc(sizeof(Config_server));

    char *end, *ptr;

    ptr = find_section(file, "[CONFIG]", &end);

    if ( find_field_n(ptr, "port=", end, &config->port ) ) return NULL;
    if ( find_field_n(ptr, "hosts=", end, &config->hosts ) ) return NULL;
    if ( find_field_n(ptr, "maxcycle=", end, &config->maxcycle ) ) return NULL;
	if ( find_field_n(ptr, "php_port=", end, &config->phpport ) ) return NULL;
	if ( find_field_n(ptr, "timeout=", end, &config->keepTimeOut ) ) return NULL;
	if ( find_field_n(ptr, "maxalive=", end, &config->keepMax ) ) return NULL;

    config->websites = malloc( sizeof(Website) * config->hosts);

    char name[15];
    for (int i = 0; i < config->hosts; i++)
    {   
        sprintf(name, "[HOST:%d]",i);
        ptr = find_section(file, name, &end);

        if (!(ptr)) return NULL;

        if ( find_field_s(ptr, "name=", end, &config->websites[i].name, &config->websites[i].name_len ) ) return NULL;
        if ( find_field_s(ptr, "root=", end, &config->websites[i].root, &config->websites[i].root_len ) ) return NULL;
        if ( find_field_s(ptr, "index=", end, &config->websites[i].index, &config->websites[i].index_len ) ) return NULL;
    }
    return config;
}

Website *find_website(Config_server *config, char *host, int len){
	int count = config->hosts;
	for (int i = 0; i < count; i++)
	{
		if ( strncmp(host, config->websites[i].name, len) == 0) return &config->websites[i];
	}
	return NULL;
}

void generate_config_file(){
    FILE *file = open_file("server.ini","w");
    fprintf(file,"[CONFIG]\nport=8080  #Listen Port\nphp_port=9000 #Port of the php server\nhosts=1  #Numbers of host\nmaxcycle=0   #How many request you want to process before closing the server\ntimeout=5\nmaxalive=15\n\n[HOST:0] #The number must start from 0 to hosts - 1\nname= #Name of the website\nroot= #root path (from http-server folder of absolute path)\nindex= #Default file to open if none is selected\n");
    fclose(file);
}

void free_config(Config_server *config){
    for (int i = 0; i < config->hosts; i++)
    {
        free(config->websites[i].index);
        free(config->websites[i].name);
        free(config->websites[i].root);
    }
    free(config->websites);
    free(config);
}

char *find_section(char *start, char *name, char **end){
    char *pos = strstr( start, name);

    if(pos) pos += strlen(name);
    else return pos;

    char *temp = pos;
    int cr_count = 0;

    while (cr_count < 2 && *temp != '\0')
    {
        if (*temp == '\n' ) cr_count++;
        else if (cr_count > 0) cr_count--;

        temp++;
    }
    *end = temp;
    return pos;
}

int find_field_s(char *start, char *name, char *end, char **dest, int *len){
    char *pos = strstr( start, name);

    pos += strlen(name);
    char *temp = pos;

    while ( temp < end && *temp != '\0' && *temp != '#' && *temp != '\n' && *temp != ' ') temp++;

    if ( temp >= end || *temp == '\0') return 1;    

    *len = temp - pos;

    *dest = malloc(*len + 1);
	memset(*dest, '\0', *len + 1);
    strncpy( *dest, pos, *len);
    return 0;
}

int find_field_n(char *start, char *name, char *end, int *dest){
    char *pos = strstr( start, name);
    pos += strlen(name);

    if ( end <= pos ) return 1;

    *dest = strtol(pos, &end, 10);

    return end == pos ? 1 : 0;
}