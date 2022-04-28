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
		int keepTimeOut;
		int keepMax;
    }Config_server;


    Config_server *get_config(char *file_name);
    
    void free_config(Config_server *config);

    void generate_config_file();

	Website *find_website(Config_server *config, char *host, int len);

#endif




