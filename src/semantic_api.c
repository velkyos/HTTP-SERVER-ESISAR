/**
 * @file semantic_api.c
 * @author PEDER LEO
 * @brief Semantic check of an HTTP request.
 * @version 0.1
 * @date 2022-04-8
 *
 *
 */

#include "semantic_api.h"
#include "syntax_api.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Constants */
#define S_VALID 0
#define S_NON_VALID 1
//Methode à implementer
#define S_METHOD {"GET","HEAD","POST"}
#define S_METHOD_NB 3

//Headers à implementer
#define S_HEADER {"Transfer-Encoding-header","Cookie-header","Referer-header", "User-Agent-header", "Accept-header", "Accept-Encoding-header", "Content-Length-header","Host-header","Connection-header"}
#define S_HEADER_NB 9

//Version dispo
#define S_VERSION {"HTTP/1.0","HTTP/1.1"}
#define S_VERSION_NB 2

//connection-option dispo
#define S_CONNECTION {"keep-alive","close"}
#define S_CONNECTION_NB 2


int semantic(){
  if( getRootTree() == NULL) return 0;

  _Token *methode=NULL, *version=NULL, *host=NULL;
  char *HTTP_version,*Host, *Method;
  int l,resultat_sem=S_VALID;

  /*VERIFICATION METHODE*/
  methode=searchTree(NULL,"method");
  Method=(methode!=NULL)?getElementValue(methode->node,&l):NULL;
  resultat_sem+=method(Method);
  //printf("Method valide ? %d\n",method(Method));
  purgeElement(&methode);

  /*VERIFICATION HEADER */
  resultat_sem+=header_unique();
  //printf("Header unique valide? %d\n",header_unique());

  /* VERIFICATION DE LA VERSION ET HEADER HOST*/
  version=searchTree(NULL,"HTTP-version");
  host=searchTree(NULL,"Host");

  HTTP_version=(version!=NULL)?getElementValue(version->node,&l):NULL;
  Host=(host!=NULL)?getElementValue(host->node,&l):NULL;
  resultat_sem+=http_version(HTTP_version,Host);
  //printf("Version valide ? %d\n",http_version(HTTP_version,Host));
  purgeElement(&version);
  purgeElement(&host);

  /*HEADER CONNECTION*/
  resultat_sem+=connection();
  //printf("Connection-header valide ? %d\n",connection());

  /*CONTENT-LENGH HEADER*/
  resultat_sem+=content_length();
  //printf("Content-Length valid ? %d\n",content_length());

  resultat_sem=(!resultat_sem)?1:0;
  //printf("\nresultat final %d\n",resultat_sem);
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

int header_unique(){
  int res=S_VALID,i=0,nb_header=0;
  char *header_dispo[]=S_HEADER;
  _Token *current_header, *header;
  while(!res && i<S_HEADER_NB){
    header=searchTree(NULL,header_dispo[i]);
    current_header=header;
    while(current_header!=NULL){ //Pour chaque header present dans la requete
      current_header=current_header->next;
      nb_header++;
    }
    purgeElement(&header);
    if(nb_header>1) res=S_NON_VALID; //Verfie que chaque header dispo est present 0 ou 1 fois
    nb_header=0;
    i++;
  }
  return res;
}


int connection(){
  int i=0,res=S_NON_VALID,l;
  _Token *connect=searchTree(NULL,"connection-option");
  _Token *header_connec=searchTree(NULL,"Connection-header");
  if(header_connec==NULL) return S_VALID; //Si l'header est pas present = C'est bon
  char *connection_dispo[]=S_CONNECTION;
  while(res && i<S_CONNECTION_NB){
    if(compare_string(getElementValue(connect->node,&l),connection_dispo[i])) res=S_VALID;
    i++;
  }
  purgeElement(&connect);
  purgeElement(&header_connec);
  return res;
}

int content_length(){
  _Token * content_length=searchTree(NULL,"Content-Length");
  _Token * transfert_encoding=searchTree(NULL,"Transfer-Encoding");
  int res=content_length!=NULL && transfert_encoding!=NULL;
  purgeElement(&content_length);
  purgeElement(&transfert_encoding);
  return res;

}
