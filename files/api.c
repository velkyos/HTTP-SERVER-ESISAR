#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api.h"

#include "syntax_check.h"


void recursif_search_tree(derivation_tree *start, char *name, _Token *list);
void add_token_to_list(_Token *list, derivation_tree *node);

derivation_tree *root = NULL;

int parseur(char *req, int len){
	root = create_tree_node("HTTP-message",req,len,0);
	linked_child *root_list = NULL;
	abnf_rule *rule = get_abnf_rule("HTTP-message", 12);

	int n = check_for_syntax(req, &root_list, rule->description, NULL, 0);

	root->children = root_list;
	if (root->children == NULL) printf("No results\n");

	if ( n == len ) {
		return n;
	}
	else {
		return 0;
	}
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

	if ( strcmp (start_node->tag , name) == 0) list->node = start_node;
	else recursif_search_tree( start_node, name, list);
	if (list->node == NULL && list->next == NULL)
	{
		free(list);
		printf("Didn't Find any %s !\n", name);
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

void purgeTree(void *root){
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
		if ( strcmp (ptr_node->tag , name) == 0)
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