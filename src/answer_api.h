#ifndef ANSWER_API_H
    #define ANSWER_API_H

	#define UTI_STATUS 1
	#define UTI_HEADER 2
	#define UTI_BODY 3
	#define UTI_CANFREE 1

	#include "derivation_tree.h"
	#include "utils.h"

	typedef struct st_answer_list{
		int tag;
		char *value;
		int len;
		int canFree;
		struct st_answer_list *next;
	} Answer_list;


	char * concat_answer(Answer_list *answer, int *length);
	
	void add_node_answer(Answer_list **list, int tag, char *value, int len, int canFree);

	void purge_answer(Answer_list **list);

#endif