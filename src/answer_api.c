/**
 * @file answer_api.c
 * @author MANDON ANAEL
 * @brief Generation of an HTTP answer.
 * @version 0.1
 * @date 2022-04-8
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "answer_api.h"

#include <stdlib.h> //Malloc
#include <string.h> //strcpy

char * concat_answer(Answer_list *answer, int *length){
	Answer_list *temp = answer;
	*length = 3; //For the two \n before body and null Char


	while (temp != NULL)
	{
		*length = *length + temp->len + 1;
		temp = temp->next;
	}

	char *message = malloc(*length);
	memset(message, '\0', *length);

	temp = answer;
	while ( temp != NULL && temp->tag != UTI_STATUS) temp = temp->next;
	if (temp == NULL) return NULL;
	else {
		strncpy(message, temp->value, temp->len);
		strcat(message, "\n");
	}

	temp = answer;
	while ( temp != NULL && temp->tag != UTI_HEADER) temp = temp->next;
	if (temp != NULL){
		while ( temp != NULL && temp->tag == UTI_HEADER)
		{
			strncat(message, temp->value, temp->len);
			strcat(message, "\n");
			temp = temp->next;
		}
	}
	
	temp = answer;
	while ( temp != NULL && temp->tag != UTI_BODY) temp = temp->next;
	if (temp == NULL) return NULL;
	else {
		strcat(message, "\n");
		char *t = strstr(message,"\n\n") + 2;
		memcpy(t, temp->value, temp->len);
	}


	return message;
}

void add_node_answer(Answer_list **list, int tag, char *value, int len, int canFree){
	Answer_list *new = malloc(sizeof(Answer_list));
	new->tag = tag;
	new->value = value;
	new->len = len;
	new->canFree = canFree;
	new->next = NULL;

	Answer_list *temp = *list;

	if( temp == NULL) *list = new;
	else{
		while ( temp->next != NULL ) temp = temp->next;

		temp->next = new;
	}
}

void purge_answer(Answer_list **list){
	Answer_list *temp = *list;
	Answer_list *current = NULL;

	if (*list != NULL){
		while( temp != NULL){
			current = temp;
			temp = temp->next;

			if( current->canFree == UTI_CANFREE) free(current->value);
			free(current);
		}
	}
}