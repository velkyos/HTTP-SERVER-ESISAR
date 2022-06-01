/**
 * @file syntax_check.c
 * @author BENJAMIN ROBERT | MANDON ANAEL | PEDER LEO
 * @brief Syntax check of HTTP request.
 * @version 1
 * @date 2022-03-30
 * 
 * 
 */

/* System Includes */

#include <stdio.h>    //Printf
#include <stdlib.h>   //strtol , exit
#include <ctype.h>    //tolower 
#include <string.h>   //Many string function

/* User Includes */

#include "syntax_check.h"
#include "derivation_tree.h"

/* Constants */

#define S_DEBUG 0
#define S_DEBUG_PATH 0
#define S_DEBUG_TOKEN 0
#define S_OR 47
#define S_STRING 34
#define S_SPACE 32
#define S_NOT_VALID -1
#define MAX_INT 2147483647


/* Declaration */

static abnf_rule rules[];

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

/* Definition */

int check_for_syntax(const char *request, linked_child **current_list, const char *rule_descr, const char *rule_end, int level){
	derivation_tree *current_node = NULL;
	linked_child *temp_list = NULL; //A temporary list, we only add this list to current_list at the end if all the rule is good. 

	int token_length = 0;
	int next_token_length = -1;
	int is_valid = 0;

	char *next_pos = NULL; //The pointer to the next position in the abnf rule.

	if ( get_end_or_group( rule_descr, rule_end) != NULL ) //We check if we have a OR in this group  A / B is yes but A ( A / B ) is no.
	{
		token_length = handle_or_rule(request, &temp_list, rule_descr, rule_end, level, &next_pos);

		is_valid = token_length != S_NOT_VALID;
		rule_descr = next_pos;
	}
	while (reach_str_end(rule_descr, rule_end) == 0)
	{
		if ( S_DEBUG ) printf("[%d] |%c| RULE : |%.*s|\n", level, *request, (int)(rule_end-rule_descr),rule_descr);

		next_pos = NULL;
		next_token_length = -1;
		is_valid = 0;
		

		if ( *rule_descr == S_STRING){

			if ( S_DEBUG_PATH ) printf("Path -> String\n");
			next_token_length = handle_terminal_string_rule( request, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- String : %d\n", next_token_length);

		}else if ( *rule_descr == '%') {

			if ( S_DEBUG_PATH ) printf("Path -> Number\n");
			next_token_length = handle_terminal_number_rule( (unsigned char *)request, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Number : %d\n", next_token_length);

		}else if ( *rule_descr == '*' || (*rule_descr >= '0' && *rule_descr <= '9' ) ) {
				
			if ( S_DEBUG_PATH ) printf("Path -> Repetition\n");
			next_token_length = handle_repetition_rule( request, &temp_list, rule_descr, rule_end, level, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Repetition : %d\n", next_token_length);

		}else if ( *rule_descr == '[') {
			
			if ( S_DEBUG_PATH ) printf("Path -> Option\n");
			next_token_length = handle_optional_rule( request, &temp_list,  rule_descr, rule_end, level, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Option : %d\n", next_token_length);

		}else if ( *rule_descr == '(') {

			if ( S_DEBUG_PATH ) printf("Path -> Group\n");
			next_token_length = handle_group_rule( request, &temp_list, rule_descr, rule_end, level, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Group : %d\n", next_token_length);

		}else if ( *rule_descr == '$') {

			if ( S_DEBUG_PATH ) printf("Path -> Message Body\n");
			next_token_length = 0;
			if ( S_DEBUG_PATH ) printf("Path <- Message Body : %d\n", next_token_length);

		}else if ( *rule_descr != '\0' && rule_descr != rule_end){ 
			
			if ( S_DEBUG_PATH ) printf("Path -> Embedeed\n");
			int length = get_end_rule( rule_descr, rule_end) - rule_descr + 1;  //We Get the length of the name of the embedeed Rule.
			abnf_rule *new_rule = get_abnf_rule( rule_descr, length); //We get the new abnf rule.

			current_node = create_tree_node(new_rule->name, request, 0, level + 1);
			//The only part where we create a new node
			next_token_length = check_for_syntax( request, &(current_node->children), new_rule->description, NULL, level + 1);

			if (next_token_length != S_NOT_VALID){
			 	next_pos = get_next_rule(rule_descr, rule_end);
				current_node->value_length = next_token_length;
				add_child_to_list(&temp_list, current_node);
			} 
			else {
				purge_tree_node(current_node);
			}

			if ( S_DEBUG_PATH ) printf("Path <- Embedeed : %d\n", next_token_length);
		}
		
		is_valid = next_token_length != S_NOT_VALID;
		
		if (is_valid)
		{
			token_length += next_token_length;
			request += next_token_length;
		}
		rule_descr = next_pos;  // Is NULL we reach the end of the request or an Error
	}
	
	if ( is_valid == 0 )
	{
		purge_linked_children(&temp_list); //We delete all the nodes
		if ( S_DEBUG_TOKEN ) printf("[%d] Token (%d)\n", level, S_NOT_VALID);
		return S_NOT_VALID;
	}
	else
	{	
		add_list_to_list(current_list, temp_list); //We add all the nodes
		if ( S_DEBUG_TOKEN ) printf("[%d] Token (%d)\n", level, token_length);
		return token_length;
	}
	
}

abnf_rule *get_abnf_rule(const char *name, int name_length){
	abnf_rule *temp_rules = rules;
	while ( temp_rules->name != NULL)
	{
		if( strncmp( temp_rules->name, name, name_length) == 0){
			if ( strlen(temp_rules->name) == name_length) return temp_rules;
		}
		temp_rules++;
	}
	printf("Error on abnf rules !\n");
	exit(EXIT_FAILURE);
}

int reach_str_end(const char *str, const char *str_end){
	if ( str_end != NULL && str != NULL) return str > str_end;
	else if (str == NULL) return 1;
	else return 0;
}

char *cs_strchr(const char *str, char c, const char *str_end){
    str = strchr(str,c);
	return reach_str_end(str, str_end) ? NULL : (char *)str;
}

char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end){
    str_1 = strpbrk(str_1,str_2);
	return reach_str_end(str_1, str_end) ? NULL : (char *)str_1;
}

char *get_end_rule(const char *str, const char *str_end){

	while ( *str == '*' || (*str >= '0' && *str <= '9' ) ) str++;

	switch ( *str)
	{
	case '[':
		str = get_end_group(str + 1,str_end,'[',']');
		break;
	case '(':
		str = get_end_group(str + 1,str_end,'(',')');
		break;
	case '\"':
		str = cs_strchr(str + 1, '\"', str_end);
		break;
	default:
	    str = cs_strpbrk(str, " )]",str_end);
		if ( str != NULL) --str;
		break;
	}
	return (char *)str;
}

char *get_next_rule(const char *str, const char *str_end){
    char *n_str = get_end_rule(str, str_end);
	if (n_str != NULL) {
		n_str++;
		while ( *n_str == S_SPACE) n_str++;
		if (n_str != NULL && ( *n_str == '\0' || *n_str == ']' || *n_str == ')')) n_str = NULL;
	}
	return  n_str;
}

char *get_start_rule(const char *str, const char *str_end){
	if ( *str == '[' || *str == '(')
	{
		str++;
		while ( *str == S_SPACE) str++;
	}
	else if ( *str == S_STRING) str++;
	else if ( *str == '%') str += 2;

	return reach_str_end(str, str_end) ? NULL : (char *)str;
}

char *get_end_group(const char *str, const char *str_end, char open, char close){
	int in_block_cnt = 1;
	while (!reach_str_end( str, str_end) && *str != '\0')
	{
		if ( *str == open) in_block_cnt++; 
		else if( *str == close && in_block_cnt > 0) in_block_cnt--;

		if (*str == close && in_block_cnt == 0) return (char *)str; // Success

		str++;
	}
	// Failure
	return NULL;
}

char *get_end_or_group(const char *str, const char *str_end){
	while ( !reach_str_end(str,str_end) && *str != '/')
	{
		str = get_next_rule(str, str_end);
	}
	if (reach_str_end(str,str_end)) return NULL;
	return (char *)str;
}

int handle_terminal_number_rule(const unsigned char *request, const char *rule, const char *rule_end, char **next_srt){

	char *s_str = get_start_rule(rule, rule_end); //rule is on % so we go to the number
	int token_length = 0;

	unsigned char val = (unsigned char)strtol(s_str, NULL, 16);

	char *rule_dash = cs_strchr(s_str, '-', rule_end);
	char *rule_point = cs_strchr(s_str, '.', rule_end);
	
	if ( rule_dash != NULL) /* Case of %xxx-xx */
	{
		unsigned char val_2 = (unsigned char)strtol(rule_dash + 1, NULL, 16);
		if (*request >= val && *request <= val_2) token_length ++;	
	}
	else if ( rule_point != NULL) /* Case of %xxx.xx.xx.xx */
	{
		if (val == *request ){
			token_length++;
			request++;

			while ( rule_point != NULL && token_length > 0)
			{
				val = (unsigned char)strtol(++rule_point, NULL, 16);
				

				token_length = (val == *request) ? token_length + 1 : 0;

				rule_point = cs_strchr(rule_point, '.', rule_end);
				request++;
			}

		}
	}
	else //Simple case 
	{	
		if ( val == *request ) token_length ++;	
	}

	if (token_length > 0)
	{
		*next_srt = get_next_rule( rule, rule_end);
		return token_length;
	}
	return S_NOT_VALID;
}

int handle_terminal_string_rule(const char *request, const char *rule, const char *rule_end, char **next_srt){
	char *s_str = get_start_rule(rule, rule_end);
    char *e_str = get_end_rule(rule, rule_end) - 1;

	int isSame = 1;
	int token_length = 0;

	while ( isSame && !reach_str_end(s_str,e_str))
	{
		if (*request != '\0') isSame = tolower(*request) == tolower(*s_str);
		else isSame = 0;
		token_length++;
		request++;
		s_str++;
	}
	if ( isSame )
	{
		*next_srt = get_next_rule(rule, rule_end);
		return token_length;
	}
	return S_NOT_VALID;
}

int handle_or_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt){
	linked_child *list = NULL;
	int token_length = S_NOT_VALID;
	int is_valid = 0;

	char *or_pos = get_end_or_group(rule, rule_end);
	*next_srt = NULL;

	while (!reach_str_end(rule, rule_end) && token_length == S_NOT_VALID)
	{
		if ( or_pos == NULL) token_length = check_for_syntax(request, &list, rule, NULL, level);
		else token_length = check_for_syntax(request, &list, rule, or_pos - 1, level);
		
		is_valid = token_length != S_NOT_VALID;

		if (!is_valid)
		{	
			purge_linked_children(&list);
			if ( or_pos == NULL) rule = NULL;
			else{
		    	rule = get_next_rule(or_pos + 1, rule_end);
				or_pos = get_end_or_group(rule, rule_end);
			}
		}
	}
	add_list_to_list(current_list, list);
	return token_length;
}

int handle_repetition_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt){
	int min = 0, max = MAX_INT;
	char *t_str = (char *)rule;
	char *temp_pos = NULL;
	char *e_str = NULL;

	if (*t_str != '*') {
		min = strtol(t_str, &temp_pos, 10); //We get the minimal amount.
		t_str = temp_pos;
	}

	if (*t_str == '*'){ //We get the max amount.
		max = strtol( t_str + 1, &temp_pos, 10);
		if (temp_pos == t_str + 1) max = MAX_INT; //Case of * not present so an extact amount of repetition.
		t_str = temp_pos; 
	}else max = min;
	
    e_str = get_end_rule(t_str, rule_end) + 1;

	int token_length = handle_repetition(request, current_list, t_str, e_str, min, max, level);

	if ( token_length != S_NOT_VALID) *next_srt = get_next_rule(rule, rule_end);
	
	return token_length;
}

int handle_repetition(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int min, int max, int level){

	int n = 0;
	int token_length = 0;
	int next_token_length = 0;
	while ( n < max && next_token_length != S_NOT_VALID)
	{	
		next_token_length = check_for_syntax(request, current_list, rule, rule_end, level);
		
		if ( next_token_length != S_NOT_VALID )
		{
			n++;
			token_length += next_token_length;
			request += next_token_length;
		}
	}

	return (n >= min && n <= max) ? token_length :  S_NOT_VALID;
}

int handle_optional_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt){
	char *s_str = get_start_rule(rule, rule_end);
    char *e_str = get_end_rule(rule, rule_end);

	int token_length = handle_repetition(request, current_list, s_str, e_str, 0, 1, level);

	if ( token_length != S_NOT_VALID ) *next_srt = get_next_rule(rule, rule_end);
	
	return token_length;
}

int handle_group_rule(const char *request, linked_child **current_list, const char *rule, const char *rule_end, int level, char **next_srt){
	char *s_str = get_start_rule(rule, rule_end);
    char *e_str = get_end_rule(rule, rule_end);
	int token_length = check_for_syntax(request, current_list, s_str, e_str, level);

	if ( token_length != S_NOT_VALID) {
		*next_srt = get_next_rule(rule,rule_end);
	}

	return token_length;
}

/* Abnf_rules */

static abnf_rule rules[] = {

	// Specials sets
	{"ALPHA","%x41-5A / %x61-7A "},
	{"BIT"," \"0\" / \"1\" "},
	{"CHAR","%x01-7F "},
	{"VCHAR","%x21-7E "},
	{"OCTET","%x00-FF "},
	{"DIGIT","%x30-39 "},
	{"HEXDIG","DIGIT / \"A\" / \"B\" / \"C\" / \"D\" / \"E\" / \"F\" "},
	// Specials Characters
	{"CR","%x0D "},
	{"CRLF","CR LF "},
	{"HTAB","%x09 "},
	{"LF","%x0A "},
	{"SP","%x20 "},
	{"DQUOTE","%x22 "},
	{"WSP","SP / HTAB "},
	// HTTP ABNF
	{"URI","scheme \":\" hier-part [\"?\" query] [\"#\" fragment] "},
	{"hier-part","\"//\" authority path-abempty / path-absolute / path-rootless / path-empty "},
	{"URI-reference","URI / relative-ref "},
	{"absolute-URI","scheme \":\" hier-part [\"?\" query] "},
	{"relative-ref","relative-part [\"?\" query] [\"#\" fragment] "},
	{"relative-part","\"//\" authority path-abempty / path-absolute / path-noscheme / path-empty "},
	{"scheme","ALPHA *(ALPHA / DIGIT / \"+\" / \"-\" / \".\") "},
	{"authority","[userinfo \"@\"] host [\":\" port] "},
	{"userinfo","*(unreserved / pct-encoded / sub-delims / \":\") "},
	{"host","IP-literal / IPv4address / reg-name "},
	{"port","*DIGIT "},
	{"IP-literal","\"[\" (IPv6address / IPvFuture) \"]\" "},
	{"IPvFuture","\"v\" 1*HEXDIG \".\" 1*(unreserved / sub-delims / \":\") "},
	{"IPv6address","6(h16 \":\") ls32 / \"::\" 5(h16 \":\") ls32 / [h16] \"::\" 4(h16 \":\") ls32 / [h16 *1(\":\" h16)] \"::\" 3(h16 \":\") ls32 / [h16 *2(\":\" h16)] \"::\" 2(h16 \":\") ls32 / [h16 *3(\":\" h16)] \"::\" h16 \":\" ls32 / [h16 *4(\":\" h16)] \"::\" ls32 / [h16 *5(\":\" h16)] \"::\" h16 / [h16 *6(\":\" h16)] \"::\" "},
	{"h16","1*4HEXDIG "},
	{"ls32","(h16 \":\" h16) / IPv4address "},
	{"IPv4address","dec-octet \".\" dec-octet \".\" dec-octet \".\" dec-octet "},
	{"dec-octet","\"25\" %x30-35 / \"2\" %x30-34 DIGIT / \"1\" 2DIGIT / %x31-39 DIGIT / DIGIT "},
	{"reg-name","*(unreserved / pct-encoded / sub-delims) "},
	{"path","path-abempty / path-absolute / path-noscheme / path-rootless / path-empty "},
	{"path-abempty","*(\"/\" segment) "},
	{"path-absolute","\"/\" [segment-nz *(\"/\" segment)] "},
	{"path-noscheme","segment-nz-nc *(\"/\" segment) "},
	{"path-rootless","segment-nz *(\"/\" segment) "},
	{"path-empty","\"\" "},
	{"segment","*pchar "},
	{"segment-nz","1*pchar "},
	{"segment-nz-nc","1*(unreserved / pct-encoded / sub-delims / \"@\") "},
	{"pchar","unreserved / pct-encoded / sub-delims / \":\" / \"@\" "},
	{"query","*(pchar / \"/\" / \"?\") "},
	{"fragment","*(pchar / \"/\" / \"?\") "},
	{"pct-encoded","\"%\" HEXDIG HEXDIG "},
	{"unreserved","ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\" "},
	{"reserved","gen-delims / sub-delims "},
	{"gen-delims","\":\" / \"/\" / \"?\" / \"#\" / \"[\" / \"]\" / \"@\" "},
	{"sub-delims","\"!\" / \"$\" / \"&\" / \"'\" / \"(\" / \")\" / \"*\" / \"+\" / \",\" / \";\" / \"=\" "},
	{"language-range","(1*8ALPHA *(\"-\" 1*8alphanum)) / \"*\" "},
	{"alphanum","ALPHA / DIGIT "},
	{"Language-Tag","langtag / privateuse / grandfathered "},
	{"langtag","language [\"-\" script] [\"-\" region] *(\"-\" variant) *(\"-\" extension) [\"-\" privateuse] "},
	{"language","2*3ALPHA [\"-\" extlang] / 4ALPHA / 5*8ALPHA "},
	{"extlang","3ALPHA *2(\"-\" 3ALPHA) "},
	{"script","4ALPHA "},
	{"region","2ALPHA / 3DIGIT "},
	{"variant","5*8alphanum / (DIGIT 3alphanum) "},
	{"extension","singleton 1*(\"-\" (2*8alphanum)) "},
	{"singleton","DIGIT / %x41-57 / %x59-5A / %x61-77 / %x79-7A "},
	{"privateuse","\"x\" 1*(\"-\" (1*8alphanum)) "},
	{"grandfathered","irregular / regular "},
	{"irregular","\"en-GB-oed\" / \"i-ami\" / \"i-bnn\" / \"i-default\" / \"i-enochian\" / \"i-hak\" / \"i-klingon\" / \"i-lux\" / \"i-mingo\" / \"i-navajo\" / \"i-pwn\" / \"i-tao\" / \"i-tay\" / \"i-tsu\" / \"sgn-BE-FR\" / \"sgn-BE-NL\" / \"sgn-CH-DE\" "},
	{"regular","\"art-lojban\" / \"cel-gaulish\" / \"no-bok\" / \"no-nyn\" / \"zh-guoyu\" / \"zh-hakka\" / \"zh-min\" / \"zh-min-nan\" / \"zh-xiang\" "},
	{"BWS","OWS "},
	{"Connection","*(\",\" OWS) connection-option *(OWS \",\" [OWS connection-option]) "},
	{"Content-Length","1*DIGIT "},
	{"HTTP-message","start-line *(header-field CRLF) CRLF [message-body] "},
	{"HTTP-name","%x48.54.54.50 "},
	{"HTTP-version","HTTP-name \"/\" DIGIT \".\" DIGIT "},
	{"Host","uri-host [\":\" port] "},
	{"OWS","*(SP / HTAB) "},
	{"RWS","1*(SP / HTAB) "},
	{"TE","[(\",\" / t-codings) *(OWS \",\" [OWS t-codings])] "},
	{"Trailer","*(\",\" OWS) field-name *(OWS \",\" [OWS field-name]) "},
	{"Transfer-Encoding","*(\",\" OWS) transfer-coding *(OWS \",\" [OWS transfer-coding]) "},
	{"Upgrade","*(\",\" OWS) protocol *(OWS \",\" [OWS protocol]) "},
	{"Via","*(\",\" OWS) (received-protocol RWS received-by [RWS comment]) *(OWS \",\" [OWS (received-protocol RWS received-by [RWS comment])]) "},
	{"absolute-form","absolute-URI "},
	{"absolute-path","1*(\"/\" segment) "},
	{"asterisk-form","\"*\" "},
	{"authority-form","authority "},
	{"chunk","chunk-size [chunk-ext] CRLF chunk-data CRLF "},
	{"chunk-data","1*OCTET "},
	{"chunk-ext","*(\";\" chunk-ext-name [\"=\" chunk-ext-val]) "},
	{"chunk-ext-name","token "},
	{"chunk-ext-val","token / quoted-string "},
	{"chunk-size","1*HEXDIG "},
	{"chunked-body","*chunk last-chunk trailer-part CRLF "},
	{"comment","\"(\" *(ctext / quoted-pair / comment) \")\" "},
	{"connection-option","token "},
	{"ctext","HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text "},
	{"field-content","field-vchar [1*(SP / HTAB) field-vchar] "},
	{"field-name","token "},
	{"field-value","*(field-content / obs-fold) "},
	{"field-vchar","VCHAR / obs-text "},
	{"http-URI","\"http://\" authority path-abempty [\"?\" query] [\"#\" fragment] "},
	{"https-URI","\"https://\" authority path-abempty [\"?\" query] [\"#\" fragment] "},
	{"last-chunk","1*\"0\" [chunk-ext] CRLF "},
	{"message-body","$ "},
	{"method","token "},
	{"obs-fold","CRLF 1*(SP / HTAB) "},
	{"obs-text","%x80-FF "},
	{"origin-form","absolute-path [\"?\" query] "},
	{"partial-URI","relative-part [\"?\" query] "},
	{"protocol","protocol-name [\"/\" protocol-version] "},
	{"protocol-name","token "},
	{"protocol-version","token "},
	{"pseudonym","token "},
	{"qdtext","HTAB / SP / \"!\" / %x23-5B / %x5D-7E / obs-text "},
	{"quoted-pair","\"\\\" (HTAB / SP / VCHAR / obs-text) "},
	{"quoted-string","DQUOTE *(qdtext / quoted-pair) DQUOTE "},
	{"rank","(\"0\" [\".\" *3DIGIT]) / (\"1\" [\".\" *3\"0\"]) "},
	{"reason-phrase","*(HTAB / SP / VCHAR / obs-text) "},
	{"received-by","(uri-host [\":\" port]) / pseudonym "},
	{"received-protocol","[protocol-name \"/\"] protocol-version "},
	{"request-line","method SP request-target SP HTTP-version CRLF "},
	{"request-target","origin-form "},
	{"start-line","request-line / status-line "},
	{"status-code","3DIGIT "},
	{"status-line","HTTP-version SP status-code SP reason-phrase CRLF "},
	{"t-codings","\"trailers\" / (transfer-coding [t-ranking]) "},
	{"t-ranking","OWS \";\" OWS \"q=\" rank "},
	{"tchar","\"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA "},
	{"token","1*tchar "},
	{"trailer-part","*(header-field CRLF) "},
	{"transfer-coding","\"chunked\" / \"compress\" / \"deflate\" / \"gzip\" / transfer-extension "},
	{"transfer-extension","token *(OWS \";\" OWS transfer-parameter) "},
	{"transfer-parameter","token BWS \"=\" BWS (token / quoted-string) "},
	{"uri-host","host "},
	{"Accept","[(\",\" / (media-range [accept-params])) *(OWS \",\" [OWS (media-range [accept-params])])] "},
	{"Accept-Charset","*(\",\" OWS) ((charset / \"*\") [weight]) *(OWS \",\" [OWS ((charset / \"*\") [weight])]) "},
	{"Accept-Encoding","[(\",\" / (codings [weight])) *(OWS \",\" [OWS (codings [weight])])] "},
	{"Accept-Language","*(\",\" OWS) (language-range [weight]) *(OWS \",\" [OWS (language-range [weight])]) "},
	{"Allow","[(\",\" / method) *(OWS \",\" [OWS method])] "},
	{"Content-Encoding","*(\",\" OWS) content-coding *(OWS \",\" [OWS content-coding]) "},
	{"Content-Language","*(\",\" OWS) Language-Tag *(OWS \",\" [OWS Language-Tag]) "},
	{"Content-Location","absolute-URI / partial-URI "},
	{"Content-Type","media-type "},
	{"Date","HTTP-date "},
	{"Expect","\"100-continue\" "},
	{"GMT","%x47.4D.54 "},
	{"HTTP-date","IMF-fixdate / obs-date "},
	{"IMF-fixdate","day-name \",\" SP date1 SP time-of-day SP GMT "},
	{"Location","URI-reference "},
	{"Max-Forwards","1*DIGIT "},
	{"Referer","absolute-URI / partial-URI "},
	{"Retry-After","HTTP-date / delay-seconds "},
	{"Server","product *(RWS (product / comment)) "},
	{"User-Agent","product *(RWS (product / comment)) "},
	{"Vary","\"*\" / (*(\",\" OWS) field-name *(OWS \",\" [OWS field-name])) "},
	{"accept-ext","OWS \";\" OWS token [\"=\" (token / quoted-string)] "},
	{"accept-params","weight *accept-ext "},
	{"asctime-date","day-name SP date3 SP time-of-day SP year "},
	{"charset","token "},
	{"codings","content-coding / \"identity\" / \"*\" "},
	{"content-coding","token "},
	{"date1","day SP month SP year "},
	{"date2","day \"-\" month \"-\" 2DIGIT "},
	{"date3","month SP (2DIGIT / (SP DIGIT)) "},
	{"day","2DIGIT "},
	{"day-name","%x4D.6F.6E / %x54.75.65 / %x57.65.64 / %x54.68.75 / %x46.72.69 / %x53.61.74 / %x53.75.6E "},
	{"day-name-l","%x4D.6F.6E.64.61.79 / %x54.75.65.73.64.61.79 / %x57.65.64.6E.65.73.64.61.79 / %x54.68.75.72.73.64.61.79 / %x46.72.69.64.61.79 / %x53.61.74.75.72.64.61.79 / %x53.75.6E.64.61.79 "},
	{"delay-seconds","1*DIGIT "},
	{"hour","2DIGIT "},
	{"media-range","(\"*/*\" / (type \"/\" subtype) / (type \"/*\")) *(OWS \";\" OWS parameter) "},
	{"media-type","type \"/\" subtype *(OWS \";\" OWS parameter) "},
	{"minute","2DIGIT "},
	{"month","%x4A.61.6E / %x46.65.62 / %x4D.61.72 / %x41.70.72 / %x4D.61.79 / %x4A.75.6E / %x4A.75.6C / %x41.75.67 / %x53.65.70 / %x4F.63.74 / %x4E.6F.76 / %x44.65.63 "},
	{"obs-date","rfc850-date / asctime-date "},
	{"parameter","token \"=\" (token / quoted-string) "},
	{"product","token [\"/\" product-version] "},
	{"product-version","token "},
	{"qvalue","(\"0\" [\".\" *3DIGIT]) / (\"1\" [\".\" *3\"0\"]) "},
	{"rfc850-date","day-name-l \",\" SP date2 SP time-of-day SP GMT "},
	{"second","2DIGIT "},
	{"subtype","token "},
	{"time-of-day","hour \":\" minute \":\" second "},
	{"type","token "},
	{"weight","OWS \";\" OWS \"q=\" qvalue "},
	{"year","4DIGIT "},
	{"ETag","entity-tag "},
	{"If-Match","\"*\" / (*(\",\" OWS) entity-tag *(OWS \",\" [OWS entity-tag])) "},
	{"If-Modified-Since","HTTP-date "},
	{"If-None-Match","\"*\" / (*(\",\" OWS) entity-tag *(OWS \",\" [OWS entity-tag])) "},
	{"If-Unmodified-Since","HTTP-date "},
	{"Last-Modified","HTTP-date "},
	{"entity-tag","[weak] opaque-tag "},
	{"etagc","\"!\" / %x23-7E "},
	{"opaque-tag","DQUOTE *etagc DQUOTE "},
	{"weak","%x57.2F "},
	{"Accept-Ranges","acceptable-ranges "},
	{"Content-Range","byte-content-range / other-content-range "},
	{"If-Range","entity-tag / HTTP-date "},
	{"Range","byte-ranges-specifier / other-ranges-specifier "},
	{"acceptable-ranges","(*(\",\" OWS) range-unit *(OWS \",\" [OWS range-unit])) / \"none\" "},
	{"byte-content-range","bytes-unit SP (byte-range-resp / unsatisfied-range) "},
	{"byte-range","first-byte-pos \"-\" last-byte-pos "},
	{"byte-range-resp","byte-range \"/\" (complete-length / \"*\") "},
	{"byte-range-set","*(\",\" OWS) (byte-range-spec / suffix-byte-range-spec) *(OWS \",\" [OWS (byte-range-spec / suffix-byte-range-spec)]) "},
	{"byte-range-spec","first-byte-pos \"-\" [last-byte-pos] "},
	{"byte-ranges-specifier","bytes-unit \"=\" byte-range-set "},
	{"bytes-unit","\"bytes\" "},
	{"complete-length","1*DIGIT "},
	{"first-byte-pos","1*DIGIT "},
	{"last-byte-pos","1*DIGIT "},
	{"other-content-range","other-range-unit SP other-range-resp "},
	{"other-range-resp","*CHAR "},
	{"other-range-set","1*VCHAR "},
	{"other-range-unit","token "},
	{"other-ranges-specifier","other-range-unit \"=\" other-range-set "},
	{"range-unit","bytes-unit / other-range-unit "},
	{"suffix-byte-range-spec","\"-\" suffix-length "},
	{"suffix-length","1*DIGIT "},
	{"unsatisfied-range","\"*/\" complete-length "},
	{"Age","delta-seconds "},
	{"Cache-Control","*(\",\" OWS) cache-directive *(OWS \",\" [OWS cache-directive]) "},
	{"Expires","HTTP-date "},
	{"Pragma","*(\",\" OWS) pragma-directive *(OWS \",\" [OWS pragma-directive]) "},
	{"Warning","*(\",\" OWS) warning-value *(OWS \",\" [OWS warning-value]) "},
	{"cache-directive","token [\"=\" (token / quoted-string)] "},
	{"delta-seconds","1*DIGIT "},
	{"extension-pragma","token [\"=\" (token / quoted-string)] "},
	{"pragma-directive","\"no-cache\" / extension-pragma "},
	{"warn-agent","(uri-host [\":\" port]) / pseudonym "},
	{"warn-code","3DIGIT "},
	{"warn-date","DQUOTE HTTP-date DQUOTE "},
	{"warn-text","quoted-string "},
	{"warning-value","warn-code SP warn-agent SP warn-text [SP warn-date] "},
	{"Proxy-Authenticate","*(\",\" OWS) challenge *(OWS \",\" [OWS challenge]) "},
	{"Proxy-Authorization","credentials "},
	{"WWW-Authenticate","*(\",\" OWS) challenge *(OWS \",\" [OWS challenge]) "},
	{"auth-param","token BWS \"=\" BWS (token / quoted-string) "},
	{"auth-scheme","token "},
	{"challenge","auth-scheme [1*SP (token68 / [(\",\" / auth-param) *(OWS \",\" [OWS auth-param])])] "},
	{"credentials","auth-scheme [1*SP (token68 / [(\",\" / auth-param) *(OWS \",\" [OWS auth-param])])] "},
	{"Authorization","credentials "},
	{"token68","1*(ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\" / \"+\" / \"/\") *\"=\" "},
	{"Connection-header","\"Connection\" \":\" OWS Connection OWS "},
	{"Content-Length-header","\"Content-Length\" \":\" OWS Content-Length OWS "},
	{"Content-Type-header","\"Content-Type\" \":\" OWS Content-Type OWS "},
	{"Trailer-header","\"Trailer\" \":\" OWS Trailer OWS "},
	{"Transfer-Encoding-header","\"Transfer-Encoding\" \":\" OWS Transfer-Encoding OWS "},
	{"Upgrade-header","\"Upgrade\" \":\" OWS Upgrade OWS "},
	{"Via-header","\"Via\" \":\" OWS Via OWS "},
	{"Age-header","\"Age\" \":\" OWS Age OWS "},
	{"Expires-header","\"Expires\" \":\" OWS Expires OWS "},
	{"Date-header","\"Date\" \":\" OWS Date OWS "},
	{"Location-header","\"Location\" \":\" OWS Location OWS "},
	{"Retry-After-header","\"Retry-After\" \":\" OWS Retry-After OWS "},
	{"Vary-header","\"Vary\" \":\" OWS Vary OWS "},
	{"Warning-header","\"Warning\" \":\" OWS Warning OWS "},
	{"Cache-Control-header","\"Cache-Control\" \":\" OWS Cache-Control OWS "},
	{"Expect-header","\"Expect\" \":\" OWS Expect OWS "},
	{"Host-header","\"Host\" \":\" OWS Host OWS "},
	{"Max-Forwards-header","\"Max-Forwards\" \":\" OWS Max-Forwards OWS "},
	{"Pragma-header","\"Pragma\" \":\" OWS Pragma OWS "},
	{"Range-header","\"Range\" \":\" OWS Range OWS "},
	{"TE-header","\"TE\" \":\" OWS TE OWS "},
	{"If-Match-header","\"If-Match\" \":\" OWS If-Match OWS "},
	{"If-None-Match-header","\"If-None-Match\" \":\" OWS If-None-Match OWS "},
	{"If-Modified-Since-header","\"If-Modified-Since\" \":\" OWS If-Modified-Since OWS "},
	{"If-Unmodified-Since-header","\"If-Unmodified-Since\" \":\" OWS If-Unmodified-Since OWS "},
	{"If-Range-header","\"If-Range\" \":\" OWS If-Range OWS "},
	{"Accept-header","\"Accept\" \":\" OWS Accept OWS "},
	{"Accept-Charset-header","\"Accept-Charset\" \":\" OWS Accept-Charset OWS "},
	{"Accept-Encoding-header","\"Accept-Encoding\" \":\" OWS Accept-Encoding OWS "},
	{"Accept-Language-header","\"Accept-Language\" \":\" OWS Accept-Language OWS "},
	{"Authorization-header","\"Authorization\" \":\" OWS Authorization OWS "},
	{"Proxy-Authorization-header","\"Proxy-Authorization\" \":\" OWS Proxy-Authorization OWS "},
	{"Referer-header","\"Referer\" \":\" OWS Referer OWS "},
	{"User-Agent-header","\"User-Agent\" \":\" OWS User-Agent OWS "},
	{"cookie-pair","cookie-name \"=\" cookie-value "},
	{"cookie-name","token "},
	{"cookie-value","(DQUOTE *cookie-octet DQUOTE) / *cookie-octet "},
	{"cookie-octet","%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E "},
	{"Cookie-header","\"Cookie:\" OWS cookie-string OWS "},
	{"cookie-string","cookie-pair *(\";\" SP cookie-pair) "},
	{"header-field","Connection-header / Content-Length-header / Referer-header / Content-Type-header / Cookie-header / Transfer-Encoding-header / Expect-header / Host-header / Accept-header / Accept-Language-header / User-Agent-header / Accept-Charset-header / Accept-Encoding-header / (field-name \":\" OWS field-value OWS) "},	{NULL,NULL}
}; /**< All abnf rules*/

