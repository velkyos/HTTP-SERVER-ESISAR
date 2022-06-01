#ifndef SYNTAX_API_H
	#define SYNTAX_API_H

	/**
	 * @file syntax_api.h
	 * @author BENJAMIN ROBERT | MANDON ANAEL | PEDER LEO
	 * @brief Syntax api between parsing and other function
	 * @version 1
	 * @date 2022-03-30
	 */

	/* System Includes */

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	/* User Includes */

	#include "syntax_check.h"

	#define S_NOT_VALID -1

	/** The root node */
	derivation_tree *root = NULL;

	/**
	 * @brief A Basic linked list for storing all node of the tree.
	 */
	typedef struct _token {
			void *node;  
			struct _token *next; 
			} _Token;

	/**
	 * @brief Get the Root Tree object.
	 * 
	 * @return The root as an opaque object.
	 */
	void *getRootTree(); 

	/**
	 * @brief Search all tree node with a tag.
	 * 
	 * @param start The start of the Tree, leave NULL to start from the root.
	 * @param name The tag we want to search
	 * @return Return a linked list of all node matching the tag.
	 */
	_Token *searchTree(void *start,char *name); 

	/**
	 * @brief Get the tag of an Element in the Linked List.
	 * 
	 * @param node The node we want info on.
	 * @param len A integer where, we are going to store the length of the tag.
	 * @return The pointer to the tag of the element.
	 */
	char *getElementTag(void *node,int *len); 

	/**
	 * @brief Get the value of an Element in the Linked List.
	 * 
	 * @param node The node we want info on.
	 * @param len A integer where, we are going to store the length of the value.
	 * @return The pointer to the value of the element.
	 */
	char *getElementValue(void *node,int *len); 

	/**
	 * @brief Purge the linked list given by searchTree.
	 * 
	 * @param r The start of the linked list.
	 */
	void purgeElement(_Token **r); 

	/**
	 * @brief Purge the tree used to store all the information about the request
	 * 
	 * @param tree The pointer to the tree
	 */
	void purgeTree(void *tree); 

	/**
	 * @brief Main function responsible of parsing a text.
	 * 
	 * @param req The pointer to the text.
	 * @param len The length of the text.
	 * @return Return 0 in case of a failure, or the length of text.
	 */
	int parser(char *req, int len); 

	/**
	 * @brief Search in the tree recursively
	 * 
	 * @param start Start of the tree.
	 * @param name The tag we are currently searching for.
	 * @param list The address of the linked list for storing results.
	 */
	void recursif_search_tree(derivation_tree *start, char *name, _Token *list);

	/**
	 * @brief A Basic function for adding a node to the linked list.
	 * 
	 * @param list The linked list.
	 * @param node The node we want to add.
	 */
	void add_token_to_list(_Token *list, derivation_tree *node);

#endif