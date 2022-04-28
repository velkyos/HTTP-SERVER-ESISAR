#ifndef PROCESS_API_H
    #define PROCESS_API_H

    #include "derivation_tree.h"
	#include "config.h"

    derivation_tree *process_request(derivation_tree *request, Config_server *config);

#endif


