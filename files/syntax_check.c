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

int reach_str_end(const char *str, const char *str_end);

char *cs_strchr(const char *str, char c, const char *str_end);

char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end);

char *get_end_rule(const char *str, const char *str_end);

char *get_next_rule(const char *str, const char *str_end);

char *get_start_rule(const char *str, const char *str_end);

char *get_end_group(const char *str, const char *str_end, char open, char close);

char *get_end_or(const char *str, const char *str_end);

char *get_end_or_group(const char *str, const char *str_end);

int handle_terminal_number_rule(char c, const char *rule, const char *rule_end, char **next_srt);

int handle_terminal_string_rule(const char *request, const char *rule, const char *rule_end, char **next_srt);

int handle_or_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt);

int handle_repetition_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt);

int handle_optional_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt);

int handle_repetition(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, int min, int max);

int handle_group_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt);

/* Definition */

int check_for_syntax(const char *request, derivation_tree *previous_node, const char *rule_descr, const char *rule_end){
	derivation_tree *current_node = NULL;
	int token_length = 0;
	int next_token_length = -1;
	int is_valid = 0;

	char *before_pos = NULL;
	char *next_pos = NULL;

	if ( get_end_or_group( rule_descr, rule_end) != NULL )
	{
		token_length = handle_or_rule(request, previous_node, rule_descr, rule_end, &next_pos);

		is_valid = token_length != S_NOT_VALID;

		rule_descr = next_pos;
	}
	while (reach_str_end(rule_descr, rule_end) == 0)
	{
		if ( S_DEBUG ) printf("RULE : %.*s\n", (rule_end-rule_descr),rule_descr);

		next_pos = NULL;
		next_token_length = -1;
		is_valid = 0;

		if ( *rule_descr == S_STRING){

			if ( S_DEBUG_PATH ) printf("Path -> String\n");
			next_token_length = handle_terminal_string_rule( request, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- String : %d\n", next_token_length);

		}else if ( *rule_descr == '%') {

			if ( S_DEBUG_PATH ) printf("Path -> Number\n");
			next_token_length = handle_terminal_number_rule( *request, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Number : %d\n", next_token_length);

		}else if ( *rule_descr == '*' || (*rule_descr >= '0' && *rule_descr <= '9' ) ) {
				
			if ( S_DEBUG_PATH ) printf("Path -> Repetition\n");
			next_token_length = handle_repetition_rule( request, previous_node, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Repetition : %d\n", next_token_length);

		}else if ( *rule_descr == '[') {
			
			if ( S_DEBUG_PATH ) printf("Path -> Option\n");
			next_token_length = handle_optional_rule( request, previous_node,  rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Option : %d\n", next_token_length);

		}else if ( *rule_descr == '(') {

			if ( S_DEBUG_PATH ) printf("Path -> Group\n");
			next_token_length = handle_group_rule( request, previous_node, rule_descr, rule_end, &next_pos);
			if ( S_DEBUG_PATH ) printf("Path <- Group : %d\n", next_token_length);

		}else{
			
			if ( S_DEBUG_PATH ) printf("Path -> Embedeed\n");
			int length = get_end_rule( rule_descr, rule_end) - rule_descr + 1;  //We Get the length of the name of the embedeed Rule.
			abnf_rule *new_rule = get_abnf_rule( rule_descr, length); //We get the new abnf rule.

			// TODO : Créer current_node  (level + 1) (Child NULL) (TAG new_rule->name) (Value -> request)

			next_token_length = check_for_syntax( request, current_node, new_rule->description, NULL);

			if (next_token_length != S_NOT_VALID){
			next_pos = get_next_rule(rule_descr, rule_end);
			} // TODO :  Add current to previous
			else ;// TODO : Delete Tree depuis current

			if ( S_DEBUG_PATH ) printf("Path <- Embedeed : %d\n", next_token_length);
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
		if ( S_DEBUG_TOKEN ) printf("Token (%d)\n",S_NOT_VALID);
		return S_NOT_VALID;
	}
	else
	{
		/* TODO : Add value to previous */

		if ( S_DEBUG_TOKEN ) printf("Token (%d)\n",token_length);
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
	return reach_str_end(str, str_end) ? NULL : str;
}

char *cs_strpbrk(const char *str_1, const char *str_2, const char *str_end){
    str_1 = strpbrk(str_1,str_2);
	return reach_str_end(str_1, str_end) ? NULL : str_1;
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
	return str;
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

	return reach_str_end(str, str_end) ? NULL : str;
}

char *get_end_group(const char *str, const char *str_end, char open, char close){
	int in_block_cnt = 1;
	while (!reach_str_end( str, str_end) && *str != '\0')
	{
		if ( *str == open) in_block_cnt++; 
		else if( *str == close && in_block_cnt > 0) in_block_cnt--;

		if (*str == close && in_block_cnt == 0) return str; // Success

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
	return str;
}

int handle_terminal_number_rule(char c, const char *rule, const char *rule_end, char **next_srt){

	char *s_str = get_start_rule(rule, rule_end); //Str is on % so we go to the number

	char val_1 = (char)strtol(s_str, NULL, 16);
	char val_2 = val_1;
	
	char *rule_dash = cs_strchr(s_str, '-', rule_end);
	char *rule_point = cs_strchr(s_str, '.', rule_end);

	if ( rule_dash != NULL) /* Case of %xxx-xx */
	{
		char val_2 = (char)strtol(rule_dash + 1, NULL, 16);
		return (c >= val_1 && c <= val_2) ? 1 : -1;

	}else if ( rule_point != NULL) /* Case of %xxx.xx.xx.xx */
	{
		while ( rule_point != NULL && val_1 != c)
		{
			val_1 = (char)strtol(++rule_point, NULL, 16);

			rule_point = cs_strchr(rule_point, '.', rule_end);
		}
		val_2 = val_1;
	}

	if ( c >= val_1 && c <= val_2 ){
		*next_srt = get_next_rule( rule, rule_end);
		return 1;
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

int handle_or_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt){
	int token_length = S_NOT_VALID;
	int is_valid = 0;

	char *or_pos = get_end_or_group(rule, rule_end);
	*next_srt = NULL;

	while (!reach_str_end(rule, rule_end) && token_length == S_NOT_VALID)
	{
		if ( or_pos == NULL) token_length = check_for_syntax(request, previous_node, rule, NULL);
		else token_length = check_for_syntax(request, previous_node, rule, or_pos - 1);
		
		is_valid = token_length != S_NOT_VALID;

		if (!is_valid)
		{	
			if ( or_pos == NULL) rule = NULL;
			else{
		    	rule = get_next_rule(or_pos + 1, rule_end);
				or_pos = get_end_or_group(rule, rule_end);
			}
		}
	}
	return token_length;
}

int handle_repetition_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt){
	int min = 0, max = MAX_INT;
	char *t_str = (char *)rule;
	char **temp_pos = &t_str;
	char *s_str = NULL;
	char *e_str = NULL;

	if (*t_str != '*') min = strtol(t_str, temp_pos, 10); //We get the minimal amount.

	t_str = *temp_pos;

	if (*t_str == '*'){ //We get the max amount.
		max = strtol( t_str + 1, temp_pos, 10);
		if (max == 0) max = MAX_INT; //Case of * not present so an extact amount of repetition.
	}else max = min;
		
	t_str = *temp_pos; 

	s_str = get_start_rule(t_str, rule_end);
    e_str = get_end_rule(t_str, rule_end) + 1;

	//printf("Reglesssss : %.*s\n", (e_str - s_str + 1), s_str);

	int token_length = handle_repetition(request, previous_node, t_str, e_str, min, max);

	if ( token_length != S_NOT_VALID) *next_srt = get_next_rule(rule, rule_end);
	
	return token_length;
}

int handle_repetition(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, int min, int max){
	int n = 0;
	int token_length = 0;
	int next_token_length = 0;
	while ( n <= max && next_token_length != S_NOT_VALID)
	{
		next_token_length = check_for_syntax(request, previous_node, rule, rule_end);
		
		if ( next_token_length != S_NOT_VALID )
		{
			n++;
			token_length += next_token_length;
			request += next_token_length;
		}
	}
	
	return (n >= min && n <= max) ? token_length :  S_NOT_VALID;
}

int handle_optional_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt){
	char *s_str = get_start_rule(rule, rule_end);
    char *e_str = get_end_rule(rule, rule_end);

	int token_length = handle_repetition(request, previous_node, s_str, e_str, 0, 1);

	if ( token_length != S_NOT_VALID ) *next_srt = get_next_rule(rule, rule_end);
	
	return token_length;
}

int handle_group_rule(const char *request, derivation_tree *previous_node, const char *rule, const char *rule_end, char **next_srt){
	char *s_str = get_start_rule(rule, rule_end);
    char *e_str = get_end_rule(rule, rule_end);
	int token_length = check_for_syntax(request, previous_node, s_str, e_str);

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
	// TEST ABNF
	{"nombre","1*DIGIT "},
	{"ponct","\",\" / \".\" / \"!\" / \"?\" / \":\" "},
	{"separateur","SP / HTAB / \"-\" / \"_\" "},
	{"debut","\"start\" "},
	{"fin","\"fin\" "},
	{"mot","1*ALPHA separateur "},
	{"message","debut 2*(mot ponct / nombre separateur) [ponct] fin LF "},
	// HTTP ABNF
	{"TEST_TEXT","\"Bonjour je suis\" SP \"ça va ?\" "},
	{"TEST","SP TEST_TEXT SP (HTAB (DQUOTE)) / 2*SP [SP] "},
	{NULL,NULL}
}; /**< All abnf rules*/