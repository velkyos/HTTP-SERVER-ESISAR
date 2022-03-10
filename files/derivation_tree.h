#ifndef DERIVATION_TREE_H
	#define DERIVATION_TREE_H

	typedef struct st_derivation_tree derivation_tree;
	typedef struct st_linked_child linked_child;

	struct st_derivation_tree
	{
		const char *tag;
		char *value;
		int value_length;
		int tree_level;
		linked_child *children;
	};

	struct st_linked_child
	{
		derivation_tree *node;
		linked_child *next;
	};
	
	derivation_tree *create_tree_node(const char *tag, char *value, int value_length, int tree_level);

	void add_child_to_node(derivation_tree *node, derivation_tree *child);

    void purge_tree_node(derivation_tree *node);

#endif