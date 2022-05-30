/**
 * @file config.c
 * @author ROBERT Benjamin & PEDER LEO & MANDON ANAEL (& Quentin Giorgi)
 * @brief Utility functions.
 * @version 0.1
 * @date 2022-04-8
 *
 */

#include "utils.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>


char *read_file(char *name, int *len){
    char *addr = NULL;
    int file;
    struct stat st;

    file=open(name,O_RDWR);

    if ( file == -1 ) {
        perror("open");
        return NULL;
    }
    if (fstat(file, &st) == -1)
        return NULL;
    if ((addr=mmap(NULL,st.st_size,PROT_WRITE | PROT_READ,MAP_PRIVATE, file, 0)) == NULL )
        return NULL;

    if (len) *len = st.st_size;

    close(file);

    return addr;
}

int write_file(char *name, char *data, int len){
  int file,res=0;
  struct stat st;

  if (stat(name, &st) == -1) res=2;

  file=open(name,O_RDWR);

  if ( file == -1 ) {
      perror("open");
      return 1;
  }
  if(pwrite(file,&data,len,0)==-1)
      return 1;

  close(file);
  return res;
}



FILE *open_file(char *name, char *option){
    FILE* file = fopen(name, option);
	if (file == NULL){
		perror("open");
    return NULL;
	}
	return file;
}


char *gmt_time(time_t *t){

	char *days[7] = {
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
	};

	char *months[12] = {
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
	};

	struct tm *g = gmtime(t);

	char *value = malloc(30* sizeof(char));
	memset(value, '\0', 30);

	sprintf( value, "%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT", days[g->tm_wday], g->tm_mday, months[g->tm_mon], 1900 + g->tm_year, g->tm_hour, g->tm_min, g->tm_sec);

	return value;
}

int compare_string(char *chaine1, char *chaine2){
  int res=1;
  int i=0;
  while(chaine1[i]>32 && chaine2[i]>32 && res){ // Les caractères ASCII inferieur 32 sont des caracteres vides (espace, \n...)
    if(chaine1[i]!=chaine2[i]) res=0;
    i++;
  }
  if(chaine2[i]!='\0') res=0; //La chaine 2 n'est pas complete
  if(chaine2[i]=='\0' && chaine1[i]>32) res=0; //La chaine 1 contient un la 2 mais pas d'espace ou autre qui suit...
  return res;
}

char *percent_encoding(char *str, int len){
	char buffer[3] = "  ";
	char *res = malloc((len + 1) * sizeof(char));
	char *temp = res;
	int index = 0;

	memset(res, '\0', len + 1 );

	while ( index < len)
	{
		if( *str == '%'){
			buffer[0] = *(++str);
			buffer[1] = *(++str);
			*temp = (char)strtol(buffer, NULL, 16);
			index += 2;
		}
		else if ( *str == '+'){
			*temp = ' ';
		}else{
			*temp = *str;
		}
		temp++;
		str++;
		index++;
	}

	return res;
}
