/**
 * @file config.c
 * @author ROBERT Benjamin (& Quentin Giorgi)
 * @brief Utility functions.
 * @version 0.1
 * @date 2022-04-8
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "utils.h"
#include <stdlib.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>


char *read_file(char *name, int *len){
    char *addr;
    int file;
    struct stat st;

    file=open(name,O_RDWR);

    if ( file == -1 ) {
        perror("open");
        return NULL;
    }
    if (fstat(file, &st) == -1)
        return NULL;
    if ((addr=mmap(NULL,st.st_size,PROT_WRITE,MAP_PRIVATE, file, 0)) == NULL )
        return NULL;


    if (len) *len = st.st_size;

    close(file);
    
    return addr;
}

FILE *open_file(char *name, char *option){
    FILE* file = fopen(name, option);
	if (file == NULL){
		printf("Error while opening %s\n", name);
		exit(-1);
	}
	return file;
}