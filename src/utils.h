#ifndef UTILS_H
    #define UTILS_H

	/**
	 * @file utils.h
	 * @author ROBERT Benjamin & PEDER LEO & MANDON ANAEL (& Quentin Giorgi)
	 * @brief Utility functions.
	 * @version 0.1
	 * @date 2022-04-8
	 *
	 */

	#include <stdio.h>
	#include "utils.h"
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/mman.h>
	#include <time.h>
	#include <string.h>

	/**
	 * @brief Read a file with mmap.
	 *
	 * @return return the content of the file and write it's len into the len param.
	 */
    char *read_file(char *name, int *len);

    /**
  	 * @brief Write a file with pwrite.
  	 *
  	 * @return Return 2 if file as been created, 0 if the file as been replaced and 1 for errors
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

	/**
	 * @brief Compare 2 string between each other
	 * 
	 * @param chaine1 
	 * @param chaine2 
	 * @return Return
	 */
	int compare_string(char *chaine1, char *chaine2);

	/**
	 * @brief Return the value of str without percent encoding
	 * +   -> ' '
	 * %20 -> ' '
	 * %2B -> '+'
	 */
	char *percent_encoding(char *str, int len);

#endif
