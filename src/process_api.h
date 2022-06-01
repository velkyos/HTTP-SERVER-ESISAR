#ifndef PROCESS_API_H
    #define PROCESS_API_H

	#include "config.h"

	#define PRO_UNKNOWN 0
	#define PRO_CLOSE 1
	#define PRO_KEEP_ALIVE 2

    char *process_request(Config_server *_config, int *anwser_len);

	int get_connection_status();

	char *get_body(int *len);

	/**
	 * @brief Get the file name (path) using the request and the config file to access the correct one.
	 * @see Config_server
	 * @see Website
	 *
	 * @return Return the Name of the file.
	 */
	char *get_file_name(char *_name);
#endif


