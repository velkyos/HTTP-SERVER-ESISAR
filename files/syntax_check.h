#ifndef SYNTAX_CHECK_H
	#define SYNTAX_CHECK_H

	#include "derivation_tree.h"

	/**
	 * @brief A basic structure, with the name and the description of each rules
	 */
	struct st_abnf_rule
	{
		const char *name;
		const char *description;
	};
	typedef struct st_abnf_rule abnf_rule;

	/**
	 * @brief Check for the syntax validity of an HTTP request.
	 * @see st_derivation_tree
	 * @see st_abnf_rule
	 * 
	 * @param request The pointer to the request. ( Must be on the first character we want to check. )
	 * @param previous_node The pointer to the previous node in the derivation tree.
	 * @param rule_descr The pointer to the rule we want to check. ( Must be on the first character we want to check. )
	 * @param rule_end The pointer to the end of the part of rule_descr we want to check, NULL if the end is the end of rule_descr.
	 * @return Return -1 if the request as a syntax error or the number of correct characters.
	 */
	int check_for_syntax(const char *request, linked_child **previous_tree, const char *rule_descr, const char *rule_end, int level);

	/**
	 * @brief Get the abnf_rule corresponding to name.
	 * 
	 * @param name 
	 * @param name_length 
	 * @return Get the pointer to the abnf_rule with the correct name.
	 */
	abnf_rule *get_abnf_rule(const char *name, int name_length);

#endif