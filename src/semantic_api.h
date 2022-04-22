#ifndef SEMANTIC_API_H
  #define SEMANTIC_API_H


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


  /*Fonction qui verifie la sémantique de la requete HTTP
  Renvoie: 1 si la semantique est correcte 0 sinon
  */
  int semantic(void *root);

  /*Fonction qui verifie la version HTTP (1.0 ou 1.1), si c'est la version 1.1 alors on verifie que
  le champ Host est bien present
  Renvoie: 1 si c'est correcte 0 sinon
  */
  int http_version(char *version,char *host);

  /*Fonction qui verifie le champ method, cette methode doit etre soit GET, HEAD ou POST
  Renvoie: 1 si c'est le cas 0 sinon
  */
  int method(char *method);

  /*Fonction qui verifie que chaque header est unique
  Renvoie: 1 si c'est le cas 0 sinon
  */
  int header_unique(void *root);

  /*Fonction qui verifie le champ connection-option est bien close ou keep-alive
  Renvoie: 1 si c'est le cas 0 sinon
  */
  int connection(void *root);

  /*Fonction qui verifie le champ Content-length et Transfer-Encoding ne sont pas present en meme temps
  Renvoie: 1 si c'est le cas 0 sinon
  */
  int content_length(void *root);

#endif
