#ifndef CONFIG_H
    #define CONFIG_H
	/**
	 * @file config.h
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
	php_port=9000 #Port du serveur fastcgi
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

	#include "utils.h"

	/**
	 * @brief Contain Basic information about each virtual host.
	 */
    typedef struct{
        char *name;
        char *root;
        char *index;
		int name_len;
		int root_len;
		int index_len;
    }Website;

	/**
	 * @brief Contain all the information needed to start and close the http-server.
	 */
    typedef struct{
        Website *websites;
        int port;
        int hosts;
        int maxcycle;
		int phpport;
		int keepTimeOut;
		int keepMax;
    }Config_server;

	/**
	 * @brief Read the config file `file_name`.
	 * @see Config_server
	 * @see Website
	 * 
	 * @param file_name 
	 * @return Return he pointer to the struct with all the config informations or NULL if an error as occured.
	 */
    Config_server *get_config(char *file_name);
    
	/**
	 * @brief Get the current website based on the host.
	 * 
	 * @param config The pointer to the current config loaded.
	 * @param host The 'name' of the website, we want to get.
	 * @param len The length of the host.
	 * @return Return the pointer to all information about this website. NULL if not found.
	 */
	Website *find_website(Config_server *config, char *host, int len);

	/**
	 * @brief Generate a default config file.
	 */
    void generate_config_file();

	/**
	 * @brief Free the config data structure.
	 * 
	 * @param config The pointer to the current config loaded.
	 */
    void free_config(Config_server *config);

	/* Static declaration */

	/**
	 * @brief Find the start of a section.
	 * 
	 * @param start The start of the part of the char* where we want to find the field.
	 * @param name The name of the section.
	 * @param end The end of the part of the char* where we want to find the field.
	 * @return Return the address of the section or NULL if the section was not found.
	 */
	static char *find_section(char *start, char *name, char **end);

	/**
	 * @brief Find the a field of type string and copy it into a dynamically allocated char *.
	 * 
	 * @param start The start of the section of the char* where we want to find the field.
	 * @param name The name of the field.
	 * @param end The end of the section of the char* where we want to find the field.
	 * @param dest The destination of the dynamically allocated char *;
	 * @param len The length of the created char*.
	 * @return Return 0 for success and 1 for failure
	 */
	static int find_field_s(char *start, char *name, char *end, char **dest, int *len);

	/**
	 * @brief Find the a field of type integer and copy it.
	 * 
	 * @param start The start of the section of the char* where we want to find the field.
	 * @param name The name of the field.
	 * @param end The end of the section of the char* where we want to find the field.
	 * @param dest The destination of the interger;
	 * @return Return 0 for success and 1 for failure
	 */
	static int find_field_n(char *start, char *name, char *end, int *dest);



#endif




