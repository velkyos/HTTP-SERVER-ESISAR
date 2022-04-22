#ifndef UTILS_H
    #define UTILS_H

    #include <stdio.h>

    char *read_file(char *name, int *len);

    FILE *open_file(char *name, char *option);
    
    int compare_string(char *chaine1, char *chaine2);

#endif
