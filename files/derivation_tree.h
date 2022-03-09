#ifndef DERIVATION_TREE_H
	#define DERIVATION_TREE_H

	typedef struct st_derivation_tree derivation_tree;
	typedef struct st_linked_child linked_child;

	struct st_derivation_tree
	{
		const char *tag;
		const char *value;
		int value_length;
		int tree_level;
		linked_child **children;
	};

	struct st_linked_child
	{
		derivation_tree *value;
		linked_child *next;
	};
	

#endif