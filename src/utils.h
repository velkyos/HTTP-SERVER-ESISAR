#ifndef UTILS_H
    #define UTILS_H

    #include <stdio.h>
	#include <time.h>

	/**
	 * @brief Read a file with mmap.
	 *
	 * @return return the content of the file and write it's len into the len param.
	 */
    char *read_file(char *name, int *len);

    /**
  	 * @brief Write a file with pwrite.
  	 *
  	 * @return return the number of bytes written in the file
  	 */
    int write_file(char *name, char *data, int len);
	/**
	 * @brief Open a file safelly
	 *
	 * @param name
	 * @param option
	 * @return The file pointer
	 */
    FILE *open_file(char *name, char *option);

	/**
	 * @brief Format the time passed in parameters to the IMF format of the HTTP protocole.
	 *
	 * @param t The time_t parameter.
	 * @return Thu, 28 Apr 2022 00:20:00 GMT  (Len 30 with null character)
	 */
	char *gmt_time(time_t *t);

	int compare_string(char *chaine1, char *chaine2);

	/**
	 * @brief Return the value of str without percent encoding
	 * +   -> ' '
	 * %20 -> ' '
	 * %2B -> '+'
	 */
	char *percent_encoding(char *str, int len);

#endif
