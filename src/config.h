#ifndef CONFIG_H
    #define CONFIG_H

    typedef struct{
        char *host_name;
        char *root_folder;
        char *index;
    }Website;


    typedef struct{
        Website *websites;
        int listen_port;
    }Config_server;


    Config_server *get_config(char *file);

#endif




