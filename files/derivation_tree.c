/* System Includes */

#include <stdlib.h> //Malloc
#include <stdio.h>

/* User Includes */

#include "derivation_tree.h"

/* Constants */

#define T_DEBUG 0

/* Declaration */

/* Definition */

derivation_tree *create_tree_node(const char *tag, char *value, int value_length, int tree_level){
	derivation_tree *node = (derivation_tree *)malloc( sizeof(derivation_tree) );
	node->tag = tag;
	node->value = value;
	node->value_length = value_length;
	node->tree_level = tree_level;
	node->children = NULL;
	return node;
}

void add_child_to_node(derivation_tree *node, derivation_tree *child){
	linked_child *ptr = NULL;
	linked_child *new_link = (linked_child *)malloc( sizeof(linked_child) );
	new_link->next = NULL;
	new_link->node = child;

	if (node->children == NULL) node->children = new_link;
	else{
		ptr = node->children;
		while ( ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = new_link;
	}
}


void purge_linked_children(derivation_tree *node){
	linked_child *ptr = node->children;

	while ( node->children != NULL)
	{
		ptr = node->children;
		node->children = ptr->next;

		purge_tree_node(ptr->node);
		ptr->node = NULL;
		ptr->next = NULL;
		free(ptr);
	}
}

void purge_tree_node(derivation_tree *node){
	if ( T_DEBUG ) printf("Purge : %s\n", node->tag);

	purge_linked_children(node);

	node->tag = NULL;
	node->value = NULL;
	node->children = NULL;
	free(node);
}