/**
 * @file config.c
 * @author ROBERT Benjamin
 * @brief Process the config file.
 * @version 1.0
 * @date 2022-04-8
 * 
 * @see Config_server
 * @see Website
 * 
 * Exemple du fichier config :
 * \code{.ini}
[CONFIG]
port=8080  #Port d'écoute
hosts=3    #Nombre de site
maxcycle=0 #Nombre de fois que le serveur va répondre à une requête, 0 si infini
timeout=5
maxalive=15

[HOST:0]
name=toto0.com
root=websites/toto0.com/
index=index0.html

[HOST:1]
name=toto1.com
root=websites/toto1.com/
index=index2.html

[HOST:2]
name=toto2.com
root=websites/toto2.com/
index=index2.html
 * \endcode
 */

/* System Includes */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* User Includes */

#include "config.h"
#include "utils.h"

/* Declaration */

int find_field_s(char *start, char *name, char *end, char **dest, int *len);
int find_field_n(char *start, char *name, char *end, int *dest);
char *find_section(char *start, char *name, char **end);

/* Definition */
/**
 * @brief Read the config file `file_name`, if not found return NULL
 * @see Config_server
 * @see Website
 * 
 * @param file_name 
 * @return NULL or the pointer to the struct with all the config infos
 */
Config_server *get_config(char *file_name){
	char *file = read_file(file_name, NULL);

    if (file == NULL) return NULL;

    Config_server *config = malloc(sizeof(Config_server));

    char *end, *ptr;

    ptr = find_section(file, "[CONFIG]", &end);

    if ( !find_field_n(ptr, "port=", end, &config->port ) ) return NULL;
    if ( !find_field_n(ptr, "hosts=", end, &config->hosts ) ) return NULL;
    if ( !find_field_n(ptr, "maxcycle=", end, &config->maxcycle ) ) return NULL;
	if ( !find_field_n(ptr, "php_port=", end, &config->phpport ) ) return NULL;
	if ( !find_field_n(ptr, "timeout=", end, &config->keepTimeOut ) ) return NULL;
	if ( !find_field_n(ptr, "maxalive=", end, &config->keepMax ) ) return NULL;

    config->websites = malloc( sizeof(Website) * config->hosts);

    char name[15];
    for (int i = 0; i < config->hosts; i++)
    {   
        sprintf(name, "[HOST:%d]",i);
        ptr = find_section(file, name, &end);

        if (!(ptr)) return NULL;

        if ( !find_field_s(ptr, "name=", end, &config->websites[i].name, &config->websites[i].name_len ) ) return NULL;
        if ( !find_field_s(ptr, "root=", end, &config->websites[i].root, &config->websites[i].root_len ) ) return NULL;
        if ( !find_field_s(ptr, "index=", end, &config->websites[i].index, &config->websites[i].index_len ) ) return NULL;
    }
    return config;
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

    if ( temp >= end || *temp == '\0') return 0;    

    *len = temp - pos;

    *dest = malloc(*len + 1);
	memset(*dest, '\0', *len + 1);
    strncpy( *dest, pos, *len);
    return 1;
}

int find_field_n(char *start, char *name, char *end, int *dest){
    char *pos = strstr( start, name);
    pos += strlen(name);

    if ( end <= pos ) return 0;

    *dest = strtol(pos, &end, 10);

    return end == pos ? 0 : 1;
}

Website *find_website(Config_server *config, char *host, int len){
	int count = config->hosts;
	for (int i = 0; i < count; i++)
	{
		if ( strncmp(host, config->websites[i].name, len) == 0) return &config->websites[i];
	}
	return NULL;
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

void generate_config_file(){
    FILE *file = open_file("server.ini","w");
    fprintf(file,"[CONFIG]\nport=8080  #Listen Port\nphp_port=9000 #Port of the php server\nhosts=1  #Numbers of host\nmaxcycle=0   #How many request you want to process before closing the server\ntimeout=5\nmaxalive=15\n\n[HOST:0] #The number must start from 0 to hosts - 1\nname= #Name of the website\nroot= #root path (from http-server folder of absolute path)\nindex= #Default file to open if none is selected\n");
    fclose(file);
}