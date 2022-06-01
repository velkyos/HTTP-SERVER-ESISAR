#ifndef SYNTAX_CHECK_H
	#define SYNTAX_CHECK_H
	/**
	 * @file syntax_check.h
	 * @author BENJAMIN ROBERT | MANDON ANAEL | PEDER LEO
	 * @brief Syntax check of HTTP request.
	 * @version 1.0
	 * @date 2022-03-30
	 * 
	 */

	/* System Includes */

	#include <stdio.h>    //Printf
	#include <stdlib.h>   //strtol , exit
	#include <ctype.h>    //tolower 
	#include <string.h>   //Many string function

	/* User Includes */

	#include "derivation_tree.h"

	/* Define */ 

	#define S_DEBUG 0
	#define S_DEBUG_PATH 0
	#define S_DEBUG_TOKEN 0
	#define S_OR 47
	#define S_STRING 34
	#define S_SPACE 32
	#define S_NOT_VALID -1
	#define MAX_INT 2147483647

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

	/**
	 * @brief A boolean function, who return 1 if str = '\0' or str > str_end.
	 * 
	 * @param str
	 * @param str_end 
	 * @return 1 or 0.
	 */
	int reach_str_end(const char *str, const char *str_end);

	/**
	 * @brief Custom version of strchr.
	 * 
	 * @param str The pointer to the string where you want to search.
	 * @param c The char you want to find.
	 * @param str_end The end of str_1.
	 * @return The pointer to the first match of c in str_1.
	 */
	char *cs_strchr(const char *str, char c, const char *str_end);

	/**
	 * @brief Custom version of the strpbrk function. With a end pointer instead of the end of str_1.
	 * 
	 * @param str_1 The pointer to the string where you want to search.
	 * @param str_2 The list of char you want to find in str_1.
	 * @param str_end The end of str_1.
	 * @return The pointer to the first match of any char from str_2 in str_1.
	 */
	char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end);

	/**
	 * @brief Get the end of the current rule.
	 * 
	 * @param str The pointer to the start of the abnf rule being checked.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @return The pointer to the end of the current rule.
	 */
	char *get_end_rule(const char *str, const char *str_end);

	/**
	 * @brief Get the start of the next rule.
	 * 
	 * @param str The pointer to the start of the abnf rule being checked.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @return The pointer to the start of the next rule.
	 */
	char *get_next_rule(const char *str, const char *str_end);

	/**
	 * @brief Get the start of the current rule.
	 * 
	 * @param str The pointer to the start of the abnf rule being checked.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @return The pointer to the start of the current rule. ( After the group start, ...)
	 */
	char *get_start_rule(const char *str, const char *str_end);

	/**
	 * @brief Get the pointer to the end of the group.
	 * 
	 * @param str The pointer to the start of the abnf rule being checked. Must be after the start of the group.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param open The char who open the group.
	 * @param close The char who end the group.
	 * @return The pointer to the end of the group.
	 */
	char *get_end_group(const char *str, const char *str_end, char open, char close);

	/**
	 * @brief Get the pointer to the end of the current or part.
	 * 
	 * @param str The pointer to the start of the abnf rule being checked.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @return The pointer to the end of the current or part.
	 */
	char *get_end_or(const char *str, const char *str_end);

	/**
	 * @brief Get the pointer to the end of the or group (When one is right).
	 * 
	 * @param str The pointer to the start of the abnf rule being checked.
	 * @param str_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @return The pointer to the end of the or group.
	 */
	char *get_end_or_group(const char *str, const char *str_end);

	/**
	 * @brief Handle all number rules. (In hexa format)
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_terminal_number_rule(const unsigned char *request, const char *rule, const char *rule_end, char **next_srt);

	/**
	 * @brief Handle all string rules.
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_terminal_string_rule(const char *request, const char *rule, const char *rule_end, char **next_srt);

	/**
	 * @brief Handle or rules. ' / '
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param current_list The pointer to current list of tree node.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param level The current level into the tree
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_or_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt);

	/**
	 * @brief Handle repetition rules. ' 4*2 '
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param current_list The pointer to current list of tree node.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param level The current level into the tree
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_repetition_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt);

	/**
	 * @brief Handle optional rules. ' [] '
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param current_list The pointer to current list of tree node.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param level The current level into the tree
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_optional_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt);

	/**
	 * @brief Handle exact repetition. Called by optional with 0 min and 1 max. Value can be from 0 to INT_MAX
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param current_list The pointer to current list of tree node.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param min How many interation minimum we want.
	 * @param max How many interation maximum we want.
	 * @param level The current level into the tree
	 * @return How many char as been validated.
	 */
	int handle_repetition(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int min, int max, int level);

	/**
	 * @brief Handle group rule '( )'
	 * 
	 * @param request The pointer to the current HTTP request.
	 * @param current_list The pointer to current list of tree node.
	 * @param rule The pointer to the start of the abnf rule being checked.
	 * @param rule_end The pointer to the end of the abnf rule being checked. Null if the end is the end of the string.
	 * @param level The current level into the tree
	 * @param next_srt The value of the rule pointer after the execution of this function.
	 * @return How many char as been validated.
	 */
	int handle_group_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt);

#endif