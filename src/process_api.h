#ifndef PROCESS_API_H
    #define PROCESS_API_H

    #include "derivation_tree.h"
	#include "config.h"

	#define PRO_UNKNOWN 0
	#define PRO_CLOSE 1
	#define PRO_KEEP_ALIVE 2

    char *process_request(derivation_tree *request, Config_server *_config, int *anwser_len);

	int get_connection_status();

#endif


