#include "answer_api.h"

char * concat_answers(Answer_list *answer, int *length){
	Answer_list *temp = answer;
	*length = 3; //For the two \n before body and null Char

	// We compute the total length
	while (temp != NULL)
	{
		*length = *length + temp->len + 1;
		temp = temp->next;
	}

	char *message = malloc(*length);
	memset(message, '\0', *length);

	// We find the status node
	temp = answer;
	while ( temp != NULL && temp->tag != A_TAG_STATUS) temp = temp->next;
	if (temp == NULL) return NULL;
	else {
		strncpy(message, temp->data, temp->len);
		strcat(message, "\n");
	}

	// We find the first header node
	temp = answer;
	while ( temp != NULL && temp->tag != A_TAG_HEADER) temp = temp->next;
	if (temp != NULL){

		// We find the new header node
		while ( temp != NULL && temp->tag == A_TAG_HEADER)
		{
			strncat(message, temp->data, temp->len);
			strcat(message, "\n");
			temp = temp->next;
		}
	}
	
	// We find the body node
	temp = answer;
	while ( temp != NULL && temp->tag != A_TAG_BODY) temp = temp->next;
	if (temp == NULL) return NULL;
	else {
		strcat(message, "\n");
		char *t = strstr(message,"\n\n") + 2;
		memcpy(t, temp->data, temp->len);
	}

	return message;
}

void add_node_answer(Answer_list **list, int tag, char *value, int len, int canFree){
	Answer_list *new = malloc(sizeof(Answer_list));
	new->tag = tag;
	new->data = value;
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

			if( current->canFree == A_CANFREE) free(current->data);
			free(current);
		}
	}
}