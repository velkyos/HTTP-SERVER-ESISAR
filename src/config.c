/**
 * @file config.c
 * @author ROBERT Benjamin
 * @brief Process the config file.
 * @version 0.1
 * @date 2022-04-8
 * 
 * @copyright Copyright (c) 2022
 * 
 * Example du fichier config :
 * \code{.ini}
[CONFIG]
port=8080  #Port d'écoute
hosts=3

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

/* Definition */

int find_field_s(char *start, char *name, char *end, char **dest);
int find_field_n(char *start, char *name, char *end, int *dest);
char *find_section(char *start, char *name, char **end);


Config_server *get_config(char *file_name){
    Config_server *config = malloc(sizeof(Config_server));

    char *file = read_file(file_name, NULL);

    char *end, *ptr;

    ptr = find_section(file, "[CONFIG]", &end);

    if ( !find_field_n(ptr, "port=", end, &config->port ) ) return NULL;
    if ( !find_field_n(ptr, "hosts=", end, &config->hosts ) ) return NULL;
    if ( !find_field_n(ptr, "maxcycle=", end, &config->maxcycle ) ) return NULL;

    config->websites = malloc( sizeof(Website) * config->hosts);

    char name[15];
    for (int i = 0; i < config->hosts; i++)
    {   
        sprintf(name, "[HOST:%d]",i);
        ptr = find_section(file, name, &end);

        if (!(ptr)) return NULL;

        if ( !find_field_s(ptr, "name=", end, &config->websites[i].name ) ) return NULL;
        if ( !find_field_s(ptr, "root=", end, &config->websites[i].root ) ) return NULL;
        if ( !find_field_s(ptr, "index=", end, &config->websites[i].index ) ) return NULL;
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

int find_field_s(char *start, char *name, char *end, char **dest){
    char *pos = strstr( start, name);

    pos += strlen(name);
    char *temp = pos;

    while ( temp < end && *temp != '\0' && *temp != '#' && *temp != '\n' && *temp != ' ') temp++;

    if ( temp >= end || *temp == '\0') return 0;    

    int n = temp - pos;

    *dest = malloc(n);
    strncpy( *dest, pos, n);
    return 1;
}

int find_field_n(char *start, char *name, char *end, int *dest){
    char *pos = strstr( start, name);
    pos += strlen(name);

    if ( end <= pos ) return 0;

    *dest = strtol(pos, &end, 10);

    return end == pos ? 0 : 1;
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