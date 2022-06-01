#ifndef SEMANTIC_API_H
  #define SEMANTIC_API_H

	/**
	 * @file semantic_api.h
	 * @author PEDER LEO
	 * @brief Semantic check of an HTTP request.
	 * @version 1
	 * @date 2022-04-8
	 */

	/* System Include */

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	/* User Include */

	#include "syntax_api.h"
	#include "utils.h"

	/* Constants */
	#define SEM_VALID 0
	#define SEM_NON_VALID 1
	//Methode à implementer
	#define SEM_METHOD {"GET","HEAD","POST"}
	#define SEM_METHOD_NB 3

	//Headers à implementer
	#define SEM_HEADER {"Transfer-Encoding-header","Cookie-header","Referer-header", "User-Agent-header", "Accept-header", "Accept-Encoding-header", "Content-Length-header","Host-header","Connection-header"}
	#define SEM_HEADER_NB 9

	//Version dispo
	#define SEM_VERSION {"HTTP/1.0","HTTP/1.1"}
	#define SEM_VERSION_NB 2

	//connection-option dispo
	#define SEM_CONNECTION {"keep-alive","close"}
	#define SEM_CONNECTION_NB 2

	/**
		 * @brief Function who check the semantic part of a request.
		 *
		 * @return Return SEM_VALID or SEM_NON_VALID.
		 */
	int semantic();


	/**
		 * @brief Function for checking the version of HTTP.
		 *
		 * @param version Pointer to the version field.
		 * @param host Pointer to the host field.
		 * @return Return 1 if valid 0 instead.
		 */
	int http_version(char *version,char *host);

	/**
	 * @brief Function for checking the method.
	 *
	 * @param method Pointer to the method field.
	 * @return Return 1 if valid 0 instead.
	 */
	int method(char *method);

	/**
	 * @brief Function for checking the unicity of header.
	 *
	 * @return Return 1 if valid 0 instead.
	 */
	int header_unique();

	/**
	 * @brief Function who verify the value of connection.
	 *
	 * @return Return 1 if valid 0 instead.
	 */
	int connection();


#endif
