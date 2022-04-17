/**
 * @file derivation_tree.c
 * @author BENJAMIN ROBERT | MANDON ANAEL | PEDER LEO
 * @brief Functions who handle the creation and deletion of the derivation_tree
 * @version 1
 * @date 2022-03-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* System Includes */

#include <stdlib.h> //Malloc
#include <stdio.h>

/* User Includes */

#include "derivation_tree.h"

/* Constants */

#define T_DEBUG 0
#define T_MAX_LEVEL 3

/* Definition */

derivation_tree *create_tree_node(const char *tag,const char *value, int value_length, int tree_level){
	derivation_tree *node = (derivation_tree *)malloc( sizeof(derivation_tree) );
	
	node->tag = tag;
	node->value = value;
	node->value_length = value_length;
	node->tree_level = tree_level;
	node->children = NULL;
	return node;
}

void add_list_to_list(linked_child **main_list, linked_child *list){
	linked_child *ptr = NULL;
	if (*main_list == NULL) *main_list = list;
	else{
		ptr = *main_list;
		while ( ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = list;
	}
	return ;
}

void add_child_to_list(linked_child **main_list, derivation_tree *child){
	linked_child *new_link = (linked_child *)malloc( sizeof(linked_child) );
	new_link->next = NULL;
	new_link->node = child;

	add_list_to_list(main_list, new_link);
	return;
}

void add_child_to_node(derivation_tree *node, derivation_tree *child){
	add_child_to_list(&(node->children), child);
	return;
}


void purge_linked_children(linked_child **main_list){
	linked_child *ptr = NULL;

	while ( *main_list != NULL)
	{
		ptr = *main_list;
		*main_list = ptr->next;

		purge_tree_node(ptr->node);
		ptr->node = NULL;
		ptr->next = NULL;
		free(ptr);
	}
	return;
}

void purge_tree_node(derivation_tree *node){
	if ( T_DEBUG ) printf("Purge : %s\n", node->tag);

	purge_linked_children(&(node->children));

	node->tag = NULL;
	node->value = NULL;
	node->children = NULL;
	free(node);
	return;
}

void print_tree(FILE *output, derivation_tree *tree){
	if (tree->tree_level < T_MAX_LEVEL) {
		linked_child *list = tree->children;
		int n = tree->tree_level;

		while ( n-- > 0) fprintf(output,"|  ");

		fprintf(output,"|---%s", tree->tag);

		if (tree->tree_level > 0) fprintf(output, " > %.*s\n", tree->value_length, tree->value);
		else fprintf(output, "\n");

		while ( list != NULL)
		{
			print_tree(output, list->node);
			list = list->next;
		}
	}
}

