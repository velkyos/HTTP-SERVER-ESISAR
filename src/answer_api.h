#ifndef ANSWER_API_H
    #define ANSWER_API_H
	/**
	 * @file answer_api.h
	 * @author MANDON ANAEL
	 * @brief Generation of an HTTP answer.
	 * @version 1
	 * @date 2022-04-8
	 */
	
	
	/* System Includes */

	#include <stdlib.h> //Malloc
	#include <string.h> //strcpy

	#define A_TAG_STATUS 1
	#define A_TAG_HEADER 2
	#define A_TAG_BODY 3
	#define A_CANFREE 1

	/**
	 * @brief A basic linked_list for storing all the parts of an HTTP status message.
	 */
	typedef struct st_answer_list{
		/** Type of this node */
		int tag;
		/** Tell the purge function, if we need to free data */
		int canFree;

		char *data;
		int len;
		struct st_answer_list *next;
	} Answer_list;

	/**
	 * @brief Concatenation of all answer nodes.
	 * @see st_answer_list
	 * 
	 * @param answer The first node of the linked list.
	 * @param length The pointer to integer where the length of the result will be stored.
	 * @return A dynamically allocated char* containing all the data stored in the linked list.
	 */
	char * concat_answers(Answer_list *answer, int *length);
	
	/**
	 * @brief Create a node and place it at the end of the linked list.
	 * @see st_answer_list
	 * 
	 * @param list The first node of the linked list.
	 * @param tag The type of the node.
	 * @param value The data of the node.
	 * @param len The length of the data.
	 * @param canFree Tell if the function purge need to free or not data.
	 */
	void add_node_answer(Answer_list **list, int tag, char *value, int len, int canFree);

	/**
	 * @brief Purge the linked list.
	 * @see st_answer_list
	 * 
	 * @param list The first node of the linked list.
	 */
	void purge_answer(Answer_list **list);

#endif