/**
 * @file semantic_api.c
 * @author PEDER LEO
 * @brief Semantic check of an HTTP request.
 * @version 0.1
 * @date 2022-04-8
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "semantic_api.h"
#include "syntax_api.h"
#include <stdio.h>
#include <string.h>

int semantic(void *root){

  _Token *methode=NULL, *version=NULL, *host=NULL;
  char *HTTP_version,*Host, *Method;
  int l,resultat_sem=1;

  /*VERIFICATION METHODE*/
  methode=searchTree(root,"method");
  Method=(methode!=NULL)?getElementValue(methode->node,&l):NULL;
  resultat_sem=method(Method);
  printf("Method valide ? %d\n",resultat_sem);

  /*VERIFICATION HEADER */
  resultat_sem=header(root);
  printf("Header valide? %d\n",resultat_sem);

  /* VERIFICATION DE LA VERSION*/
  version=searchTree(root,"HTTP-version");
  host=searchTree(root,"Host");

  HTTP_version=(version!=NULL)?getElementValue(version->node,&l):NULL;
  Host=(host!=NULL)?getElementValue(host->node,&l):NULL;
  resultat_sem=http_version(HTTP_version,Host);
  printf("Version valide ? %d\n",resultat_sem);

  return resultat_sem;
}


int http_version(char *version,char *host){
  int res=0,i=0;
  char *http_version_dispo[]=S_VERSION;
  while(!res && i<S_VERSION_NB){ //Verifie la version de HTTP 1.0 et 1.1
    if(strstr(version,http_version_dispo[i])!=NULL)res=1;
    i++;
  }
  if(res==1 && i==2 && host==NULL) res=0; //Verifie le champ Host dans la version 1.1
  return res;
}

int method(char *method){
  int res=0,i=0;
  char *method_dispo[]=S_METHOD;
  while(!res && i<S_METHOD_NB){ //Verifie la version de HTTP 1.0 et 1.1
    if(strstr(method,method_dispo[i])!=NULL)res=1;
    i++;
  }
  return res;
}

int header(void *root){
  int res=1,i=0,nb_header=0;
  char *header_dispo[]=S_HEADER;
  _Token *current_header;
  while(res==1 && i<S_HEADER_NB){
    current_header=searchTree(root,header_dispo[i]);
    while(current_header!=NULL){
      current_header=current_header->next;
      nb_header++;
    }
    if(nb_header>1) res=0;
    nb_header=0;
    i++;
  }
  return res;
}
