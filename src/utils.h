#ifndef UTILS_H
    #define UTILS_H

    #include <stdio.h>
	#include <time.h>

    char *read_file(char *name, int *len);

	char *read_file_bin(char *name, int *len);
    
    FILE *open_file(char *name, char *option);

	/**
	 * @brief Format the time passed in parameters to the IMF format of the HTTP protocole.
	 *
	 * @param t The time_t parameter.
	 * @return Thu, 28 Apr 2022 00:20:00 GMT  (Len 30 with null character)
	 */
	char *gmt_time(time_t *t);

	int compare_string(char *chaine1, char *chaine2);

#endif




