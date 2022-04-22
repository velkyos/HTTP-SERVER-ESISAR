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
#include "utils.h"
#include <stdio.h>
#include <string.h>

int semantic(void *root){

  _Token *methode=NULL, *version=NULL, *host=NULL;
  char *HTTP_version,*Host, *Method;
  int l,resultat_sem=S_VALID;

  /*VERIFICATION METHODE*/
  methode=searchTree(root,"method");
  Method=(methode!=NULL)?getElementValue(methode->node,&l):NULL;
  resultat_sem+=method(Method);
  //printf("Method valide ? %d\n",method(Method));

  /*VERIFICATION HEADER */
  resultat_sem+=header_unique(root);
  //printf("Header unique valide? %d\n",header_unique(root));

  /* VERIFICATION DE LA VERSION ET HEADER HOST*/
  version=searchTree(root,"HTTP-version");
  host=searchTree(root,"Host");

  HTTP_version=(version!=NULL)?getElementValue(version->node,&l):NULL;
  Host=(host!=NULL)?getElementValue(host->node,&l):NULL;
  resultat_sem+=http_version(HTTP_version,Host);
  //printf("Version valide ? %d\n",http_version(HTTP_version,Host));

  /*HEADER CONNECTION*/
  resultat_sem+=connection(root);
  //printf("Connection-header valide ? %d\n",connection(root));

  /*CONTENT-LENGH HEADER*/
  resultat_sem+=content_length(root);
  //printf("Content-Length valid ? %d\n",content_length(root));

  resultat_sem=(!resultat_sem)?1:0;
  printf("\nresultat final %d\n",resultat_sem);
  return resultat_sem;
}


int http_version(char *version,char *host){
  int res=S_NON_VALID,i=0;
  char *http_version_dispo[]=S_VERSION;
  while(res && i<S_VERSION_NB){ //Verifie la version de HTTP 1.0 et 1.1
    if(compare_string(version,http_version_dispo[i]))res=S_VALID;
    i++;
  }
  if(!res && i==2 && host==NULL) res=S_NON_VALID; //Verifie le champ Host dans la version 1.1
  return res;
}

int method(char *method){
  int res=S_NON_VALID,i=0;
  char *method_dispo[]=S_METHOD;
  while(res && i<S_METHOD_NB){ //Verifie la version de HTTP 1.0 et 1.1
    if(compare_string(method,method_dispo[i]))res=S_VALID;
    i++;
  }
  return res;
}

int header_unique(void *root){
  int res=S_VALID,i=0,nb_header=0;
  char *header_dispo[]=S_HEADER;
  _Token *current_header;
  while(!res && i<S_HEADER_NB){
    current_header=searchTree(root,header_dispo[i]);
    while(current_header!=NULL){ //Pour chaque header present dans la requete
      current_header=current_header->next;
      nb_header++;
    }
    if(nb_header>1) res=S_NON_VALID; //Verfie que chaque header dispo est present 0 ou 1 fois
    nb_header=0;
    i++;
  }
  return res;
}


int connection(void *root){
  int res=S_NON_VALID,i=0,l;
  _Token *connect=searchTree(root,"connection-option");
  if(searchTree(root,"Connection-header")==NULL) return S_VALID; //Si l'header est pas present = C'est bon
  char *connection_dispo[]=S_CONNECTION;
  while(res && i<S_CONNECTION_NB){
    if(compare_string(getElementValue(connect->node,&l),connection_dispo[i])) res=S_VALID;
    i++;
  }
  return res;
}

int content_length(void *root){
  return(searchTree(root,"Content-Length")!=NULL && searchTree(root,"Transfer-Encoding")!=NULL);
}
