#ifndef CONFIG_H
    #define CONFIG_H

    typedef struct{
        char *name;
        char *root;
        char *index;
    }Website;


    typedef struct{
        Website *websites;
        int port;
        int hosts;
        int maxcycle;
    }Config_server;


    Config_server *get_config(char *file_name);
    
    void free_config(Config_server *config);

#endif




