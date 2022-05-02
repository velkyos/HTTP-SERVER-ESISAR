#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syntax_api.h"

#include "syntax_check.h"

#define NOT_VALID -1

void recursif_search_tree(derivation_tree *start, char *name, _Token *list);
void add_token_to_list(_Token *list, derivation_tree *node);

derivation_tree *root = NULL;

int parser(char *req, int len){
	root = create_tree_node("HTTP-message",req,len,0); 
	abnf_rule *rule = get_abnf_rule("HTTP-message", 12); 

	int n = check_for_syntax(req, &root->children, rule->description, NULL, 0);

	//We take care of the message-body, we add the remaining part of the request.
	//If the request didn't have a message-body, we put -1 in value_length (The searchTree function will not get this node).
	if (n != NOT_VALID ) 
	{
		_Token *t = searchTree(root, "message-body");
		derivation_tree *body = (derivation_tree *)t->node;
		body->value_length = (n != NOT_VALID && len - n != 0) ? len - n : NOT_VALID;
		//printf("Message Body : %s\n", body->value);
		purgeElement(&t);
	}
	if( n == NOT_VALID) {
		purge_tree_node(root);
		root = NULL;
	}
	
	return (n != NOT_VALID) ? n : 0;
}

void *getRootTree(){
	return root;
}

_Token *searchTree(void *start,char *name){
	_Token *list = (_Token *)malloc( sizeof( _Token));
	list->node = NULL;
	list->next = NULL;

	derivation_tree *start_node = NULL;
	if ( start == NULL ) start_node = root;
	else start_node = (derivation_tree *)start;

	if ( strcmp (start_node->tag , name) == 0 && start_node->value_length != NOT_VALID) list->node = start_node;
	else recursif_search_tree( start_node, name, list);
	if (list->node == NULL && list->next == NULL)
	{
		free(list);
		//printf("Didn't Find any %s !\n", name);
		return NULL;
	}
	return list;
}

char *getElementTag(void *node,int *len){
	char *tag = (char *) ((derivation_tree *)node)->tag;
	if ( len != NULL){
		*len = strlen(tag);
	}
	return tag;
}

char *getElementValue(void *node,int *len){
	char *value = (char *) ((derivation_tree *)node )->value;
	if ( len != NULL){
		*len = ( (derivation_tree *)node )->value_length;
	}
	return value;
}

void purgeElement(_Token **r){
	_Token *ptr = *r;
	_Token *temp = NULL;

	while ( ptr != NULL)
	{
		temp = ptr;
		ptr = ptr->next;

		temp->node = NULL;
		temp->next = NULL;
		free(temp);
	}

}

void purgeTree(void *tree){
	purge_tree_node(root);
	root = NULL;
} 

void recursif_search_tree(derivation_tree *start, char *name, _Token *list){
	
	derivation_tree *ptr_node = start;
	linked_child *ptr_child = NULL;
	
	ptr_child = ptr_node->children;
	while (ptr_child != NULL)
	{
		ptr_node = ptr_child->node;
		ptr_child = ptr_child->next;
		if ( strcmp (ptr_node->tag , name) == 0  && ptr_node->value_length != NOT_VALID)
		{
			if ( list->node == NULL) list->node = ptr_node;
			else add_token_to_list(list, ptr_node);
		}
		recursif_search_tree(ptr_node, name, list);
	}
}

void add_token_to_list(_Token *list, derivation_tree *node){

	_Token *new = (_Token *)malloc( sizeof( _Token));
	new->next = NULL;
	new->node = node;

	while ( list->next != NULL) list=list->next;

	list->next = new;
}