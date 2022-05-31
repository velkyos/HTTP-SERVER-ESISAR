#ifndef CONFIG_H
    #define CONFIG_H

	/**
	 * @brief Contain Basic information about each virtual host.
	 * 
	 */
    typedef struct{
        char *name;
        char *root;
        char *index;
		int name_len;
		int root_len;
		int index_len;
    }Website;

	/**
	 * @brief Contain all the information needed to start and close the http-server.
	 * 
	 */
    typedef struct{
        Website *websites;
        int port;
        int hosts;
        int maxcycle;
		int phpport;
		int keepTimeOut;
		int keepMax;
    }Config_server;


    Config_server *get_config(char *file_name);
    
    void free_config(Config_server *config);

    void generate_config_file();

	Website *find_website(Config_server *config, char *host, int len);

#endif




