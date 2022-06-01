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
		[[CONFIG]
		port=8080  #Listen Port
		php_port=9000 #Port of the php server
		hosts=2  #Numbers of host
		maxcycle=0 #How many request you want to process before closing the server

		[HOST:0] #The number must start from 0 to hosts - 1
		name=localhost #Name of the website
		root=/var/www/html #root path (absolute path)
		index=index.html #Default file to open if none is selected
		page_404=404.html #Page to display on an error 404. Can be removed if don't have one.

		[HOST:1] #The number must start from 0 to hosts - 1
		name=hub.localhost #Name of the website
		root=/var/www/hub #root path (absolute path or from the executable)
		index=index.html #Default file to open if none is selected
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
		char *page_404;
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
	char *find_section(char *start, char *name, char **end);

	/**
	 * @brief Find the a field of type string and copy it into a dynamically allocated char *.
	 * 
	 * @param start The start of the section of the char* where we want to find the field.
	 * @param name The name of the field.
	 * @param end The end of the section of the char* where we want to find the field.
	 * @param dest The destination of the dynamically allocated char *;
	 * @return Return 0 for success and 1 for failure
	 */
	int find_field_s(char *start, char *name, char *end, char **dest);

	/**
	 * @brief Find the a field of type integer and copy it.
	 * 
	 * @param start The start of the section of the char* where we want to find the field.
	 * @param name The name of the field.
	 * @param end The end of the section of the char* where we want to find the field.
	 * @param dest The destination of the interger;
	 * @return Return 0 for success and 1 for failure
	 */
	int find_field_n(char *start, char *name, char *end, int *dest);



#endif




