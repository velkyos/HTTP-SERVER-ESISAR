#ifndef DERIVATION_TREE_H
	#define DERIVATION_TREE_H
	/**
	 * @file derivation_tree.h
	 * @author BENJAMIN ROBERT | MANDON ANAEL | PEDER LEO
	 * @brief Functions who handle the create, use and deletion of the tree based structure.
	 * @version 1.0
	 * @date 2022-03-30
	 */

	/* System Includes */

	#include <stdlib.h> //Malloc
	#include <stdio.h> //fprintf

	#define T_DEBUG 0
	#define T_MAX_LEVEL 3

	typedef struct st_derivation_tree derivation_tree;
	typedef struct st_linked_child linked_child;

	/**
	 * @brief A Tree based structure who stored all the HTTP request after parsing.
	 */
	struct st_derivation_tree
	{
		/* The Name of the current node */
		char *tag; 
		/* The Value of the current node */
		char *value;
		int value_length;
		int tree_level;
		linked_child *children;
	};

	/**
	 * @brief A basic linked list for storing all childrens of each node in the tree
	 */
	struct st_linked_child
	{
		derivation_tree *node;
		linked_child *next;
	};
	
	/**
	 * @brief Create and return the pointer to a node.
	 * 
	 * @param tag The name of the Node
	 * @param value The pointer to the start of the Value.
	 * @param value_length The length of the value.
	 * @param tree_level The current level into the tree.
	 * @return The pointer of the node.
	 */
	derivation_tree *create_tree_node(const char *tag,const char *value, int value_length, int tree_level);

	/**
	 * @brief Add a child to the children variable of the node.
	 * 
	 * @param node 
	 * @param child 
	 */
	void add_child_to_node(derivation_tree *node, derivation_tree *child);

	/**
	 * @brief Purge the tree from this node
	 * 
	 * @param node 
	 */
    void purge_tree_node(derivation_tree *node);

	/**
	 * @brief Add list to the main list.
	 * 
	 * @param main_list 
	 * @param list 
	 */
	void add_list_to_list(linked_child **main_list, linked_child *list);

	/**
	 * @brief Add one node the list of node.
	 * 
	 * @param main_list 
	 * @param child 
	 */
	void add_child_to_list(linked_child **main_list, derivation_tree *child);

	/**
	 * @brief Delete of the node from this list.
	 * 
	 * @param main_list 
	 */
	void purge_linked_children(linked_child **main_list);

	/**
	 * @brief Print the tree to a file
	 * @param output File or STD_OUT
	 * @param tree Pointer to the tree you want to display
	 */
	void print_tree(FILE *output, derivation_tree *tree);

#endif