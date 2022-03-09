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

/* Declaration */

static abnf_rule rules[];

int reach_str_end(const char *str, const char *str_end);

char *cs_strchr(const char *str, char c, const char *str_end);

char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end);

char *get_end_rule(const char *str, const char *str_end);

char *get_next_rule(const char *str, const char *str_end);

char *get_start_rule(const char *str, const char *str_end);

char *get_end_group(const char *str, const char *str_end, char open, char close);

char *get_end_or(const char *str, const char *str_end);

char *get_end_or_group(const char *str, const char *str_end);

int handle_terminal_number_rule(char c, const char *str, const char *str_end, char **next_srt);

int handle_terminal_string_rule(const char *request, const char *str, const char *str_end, char **next_srt);

int handle_or_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt);

int handle_repetition_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt);

int handle_optional_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt);

int handle_group_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt);

/* Definition */

int check_for_syntax(const char *request, derivation_tree *previous_node, const char *rule_descr, const char *rule_end){
	derivation_tree *current_node = NULL;
	int token_length = 0;
	//int before_token_length = 0;
	int next_token_length = -1;
	//int is_last_valid = -1;
	int is_valid = 0;

	//int nbr_correct_rule = 0;
	//int nbr_rule = 0;

	char *before_pos = NULL;

	//char *start_pos = NULL;
	//char *end_pos = NULL;

	char *or_pos = NULL;
	char *next_pos = NULL;

	while (reach_str_end(rule_descr, rule_end) == 0)
	{
		next_pos = NULL;
		next_token_length = -1;
		is_valid = 0;

		or_pos = get_end_or_group( rule_descr, rule_end);
		if ( or_pos != NULL )
		{
			/* TODO : Check for or and call check for each block of the or*/
		}
		else {

			if ( *rule_descr == S_STRING){
				
				next_token_length = handle_terminal_string_rule( request, rule_descr, rule_end, &next_pos);

			}else if ( *rule_descr == '%') {

				next_token_length = handle_terminal_number_rule( *request, rule_descr, rule_end, &next_pos);

			}else if ( *rule_descr == '*' || (*rule_descr >= '0' && *rule_descr <= '9' ) ) {
				
				next_token_length = handle_repetition_rule( request, previous_node, rule_descr, rule_end, &next_pos);

			}else if ( *rule_descr == '[') {

				next_token_length = handle_optional_rule( request, previous_node,  rule_descr, rule_end, &next_pos);

			}else if ( *rule_descr == '(') {

				next_token_length = handle_group_rule( request, previous_node, rule_descr, rule_end, &next_pos);

			}else{
				
				int length = get_end_rule( rule_descr, rule_end) - rule_descr;  //We Get the length of the name of the embedeed Rule.

				abnf_rule *new_rule = get_abnf_rule( rule_descr, length); //We get the new abnf rule.

				// TODO : Créer current_node  (level + 1) (Child NULL) (TAG new_rule->name) (Value -> request)

				next_token_length = check_for_syntax( request, current_node, new_rule->description, NULL);

				if (next_token_length != S_NOT_VALID) ;// TODO :  Add current to previous
				else ;// TODO : Delete Tree depuis current
			}
		}

		is_valid = next_token_length != S_NOT_VALID;
		
		if (is_valid)
		{
			token_length += next_token_length;
			request += next_token_length;
		}
		rule_descr = next_pos;  // Is NULL if we got an error so we stop
	}
	
	if ( is_valid == 0 )
	{
		/* TODO : ADD RETURN FAILED */
		return -1;
	}
	else
	{
		/* TODO : Add value to previous */
		return token_length;
	}
	
}

abnf_rule *get_abnf_rule(const char *name, int name_length){
	abnf_rule *temp_rules = rules;
	while ( temp_rules->name != NULL)
	{
		if( strncmp( temp_rules->name, name, name_length) == 0) return temp_rules;
		temp_rules++;
	}
	printf("Error on abnf rules !\n");
	exit(EXIT_FAILURE);
}

int reach_str_end(const char *str, const char *str_end){
	if ( str_end != NULL) return str > str_end;
	else if (str == NULL) return 1;
	else return 0;
}

char *cs_strchr(const char *str, char c, const char *str_end){
    str = strchr(str,c);
	return reach_str_end(str, str_end) ? NULL : str;
}

char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end){
    str_1 = strpbrk(str_1,str_2);
	return reach_str_end(str_1, str_end) ? NULL : str_1;
}

char *get_end_rule(const char *str, const char *str_end){
	switch ( *str)
	{
	case '[':
		//TO DO : Adding [] Support
		break;
	case '(':
		//TO DO : Adding () Support
		break;
	case '\"':
		str = cs_strchr(str + 1, '\"', str_end);
		break;
	default:
	    str = cs_strpbrk(str, " )]\0",str_end);
		break;
	}
	return str;
}

char *get_next_rule(const char *str, const char *str_end){
    char *n_str = get_end_rule(str, str_end);
	if (n_str != NULL) n_str++;
	return  n_str;
}

char *get_start_rule(const char *str, const char *str_end){
	if ( *str == '[' || *str == '(')
	{
		str++;
		while ( *str == S_SPACE) str++;
	}
	else if ( *str == S_STRING || *str == '%') str++;

	return reach_str_end(str, str_end) ? NULL : str;
}

char *get_end_group(const char *str, const char *str_end, char open, char close){
	printf("Not Implemented\n");
	return NULL;
}

char *get_end_or(const char *str, const char *str_end){
	printf("Not Implemented\n");
	return NULL;
}

char *get_end_or_group(const char *str, const char *str_end){
	printf("Not Implemented\n");
	return NULL;
}

int handle_terminal_number_rule(char c, const char *str, const char *str_end, char **next_srt){

	const char *t_str = str + 2; //Str is on % so we go to the number

	char val_1 = (char)strtol(t_str, NULL, 16);
	char val_2 = val_1;
	
	const char *rule_dash = cs_strchr(t_str, '-', str_end);
	const char *rule_point = cs_strchr(t_str, '.', str_end);

	if ( rule_dash != NULL) /* Case of %xxx-xx */
	{
		char val_2 = (char)strtol(rule_dash + 1, NULL, 16);
		return (c >= val_1 && c <= val_2) ? 1 : -1;

	}else if ( rule_point != NULL) /* Case of %xxx.xx.xx.xx */
	{
		while ( rule_point != NULL && val_1 != c)
		{
			val_1 = (char)strtol(++rule_point, NULL, 16);

			rule_point = cs_strchr(rule_point, '.', str_end);
		}
		val_2 = val_1;
	}

	if ( c >= val_1 && c <= val_2 ){
		*next_srt = get_next_rule( str, str_end);
		return 1;
	}
	return S_NOT_VALID;
}

int handle_terminal_string_rule(const char *request, const char *str, const char *str_end, char **next_srt){
	printf("Not Implemented\n");
	return 0;
}

int handle_or_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt){
	printf("Not Implemented\n");
	return 0;
}

int handle_repetition_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt){
	printf("Not Implemented\n");
	return 0;
}

int handle_optional_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt){
	printf("Not Implemented\n");
	return 0;
}

int handle_group_rule(const char *request, derivation_tree *previous_node, const char *str, const char *str_end, char **next_srt){
	printf("Not Implemented\n");
	return 0;
}

/* Abnf_rules */

static abnf_rule rules[] = {
	// Specials sets
	{"ALPHA","%x41-5A / %x61-7A"},
	{"BIT"," \"0\" / \"1\""},
	{"CHAR","%x01-7F"},
	{"VCHAR","%x21-7E"},
	{"OCTET","%x00-FF"},
	{"DIGIT","%x30-39"},
	{"HEXDIG","DIGIT / \"A\" / \"B\" / \"C\" / \"D\" / \"E\" / \"F\""},
	// Specials Characters
	{"CR","%x0D"},
	{"CRLF","CR LF"},
	{"HTAB","%x09"},
	{"LF","%x0A"},
	{"SP","%x20"},
	{"DQUOTE","%x22"},
	{"WSP","SP / HTAB"},
	// TEST ABNF
	{"nombre","1*DIGIT"},
	{"ponct","\",\" / \".\" / \"!\" / \"?\" / \":\""},
	{"separateur","SP / HTAB / \"-\" / \"_\""},
	{"debut","\"start\""},
	{"fin","\"fin\""},
	{"mot","1*ALPHA separateur"},
	{"message","debut ( mot ponct / nombre separateur ) [ ponct ] fin LF"},
	// HTTP ABNF

	{NULL,NULL}
}; /**< All abnf rules*/