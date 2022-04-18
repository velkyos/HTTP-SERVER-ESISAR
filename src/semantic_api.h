#ifndef SEMANTIC_API_H
  #define SEMANTIC_API_H

  //Methode à implementer
  #define S_METHOD {"GET","HEAD","POST"}
  #define S_METHOD_NB 3

  //Headers à implementer
  #define S_HEADER {"Transfer-Encoding-header","Cookie-header","Referer-header", "User-Agent-header", "Accept-header", "Accept-Encoding-header", "Content-Length-header","Host-header","Connection-header"}
  #define S_HEADER_NB 9

  #define S_VERSION {"HTTP/1.0","HTTP/1.1"}
  #define S_VERSION_NB 2

  int semantic(void *root);

  int http_version(char *version,char *host);

  int method(char *method);

  int header(void *root);

#endif
