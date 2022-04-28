/**
 * @file answer_api.c
 * @author MANDON ANAEL
 * @brief Generation of an HTTP answer.
 * @version 0.1
 * @date 2022-04-8
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "answer_api.h"
#include <stdio.h>
#include <magic.h>
#include <stdlib.h>
#include <string.h>


char * content_type(char * namefile){
  const char *magic_full;
  char * charset;
  char * res;
  magic_t magic_cookie;
  int size;

  magic_cookie=magic_open(MAGIC_MIME); //Creation magic cookie
  if(magic_cookie==NULL){
    perror("Erreur magic_open");
    return NULL;
  }

  if(magic_load(magic_cookie,NULL)!=0){ //Chargement
    perror("erreur magic_load");
    return NULL;
  }
  magic_full=magic_file(magic_cookie,namefile); //Retour la description du namefile

  if(strstr(magic_full,"text/plain;")!=NULL){ //Cas css et js qui n'est pas prix en compte pat libmagic
    charset=strstr(magic_full,"charset="); //recherche charset

    if(strstr(namefile,".css")!=NULL){ //cherche un .css
      size=strlen("text/css; ")+1+strlen(charset);
      res=(char *) malloc(size);
      strcat(strcpy(res,"text/css; "),charset);

    }else if(strstr(namefile,".js")!=NULL){ //cherche un .js
      size=strlen("text/javascript; ")+1+strlen(charset);
      res=(char *) malloc(size);
      strcat(strcpy(res,"text/javascript; "),charset);

    }
  }else{
    size=strlen(magic_full);
    res=(char *) malloc(size);
    strcpy(res,magic_full); //Convertion const char * en char *
  }
  return res;
  magic_close(magic_cookie);
  return 0;
}
