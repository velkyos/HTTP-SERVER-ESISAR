#ifndef SEMANTIC_API_H
  #define SEMANTIC_API_H


  /**
	 * @brief Fonction qui verifie la sémantique de la requete HTTP
	 *
	 * @return 1 si la semantique est correcte 0 sinon
	 */
  int semantic();


  /**
	 * @brief Fonction qui verifie la version HTTP (1.0 ou 1.1), si c'est la version 1.1 alors on verifie que le champ Host est bien present
	 *
	 * @param version pointeur vers le champ version de HTTP
   * @param host pointeur vers le (potentiel) champ host
	 * @return 1 si c'est correcte 0 sinon
	 */
  int http_version(char *version,char *host);

  /**
   * @brief Fonction qui verifie le champ method, cette methode doit etre soit GET, HEAD ou POST
   *
   * @param method pointeur vers le champ method
   * @return 1 si c'est le cas 0 sinon
   */
  int method(char *method);

  /**
   * @brief Fonction qui verifie que chaque header est unique
   *
   * @return 1 si c'est le cas 0 sinon
   */
  int header_unique();

  /**
   * @brief Fonction qui verifie le champ connection-option est bien close ou keep-alive
   *
   * @return 1 si c'est le cas 0 sinon
   */
  int connection();

  /**
   * @brief Fonction qui verifie le champ Content-length et Transfer-Encoding ne sont pas present en meme temps
   *
   * @return 1 si c'est le cas 0 sinon
   */
  int content_length();

#endif
