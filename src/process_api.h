#ifndef PROCESS_API_H
    #define PROCESS_API_H

    #include "derivation_tree.h"
	#include "config.h"



    char *process_request(derivation_tree *request, Config_server *_config, int *anwser_len);

#endif


