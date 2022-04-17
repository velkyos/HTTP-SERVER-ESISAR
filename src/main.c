/* System Includes */

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* User Includes */

#include "syntax_api.h"
#include "semantic_api.h"
#include "process_api.h"
#include "answer_api.h"
#include "derivation_tree.h"
#include "request.h"
#include "config.h"
#include "utils.h"

/* Constants */

#define false 0
#define M_PRINT_TREE 1

/* Declaration */

char error_message[] = "Code 400 : Bad Request\n";
char ok_message[] = "Code 200 : OK\n";

message *get_answer(message* request, int index);


/* Definition */

int main(int argc, char const *argv[])
{
	message *answer = NULL;
	message *request = NULL;
	Config_server *config = NULL;
	int n = 0;
	config = get_config("server.ini");

	if ( config == NULL){
		printf("Config file not found, stopping the server and generating a default one\n");
		generate_config_file();
		exit(EXIT_FAILURE);
	}

	while ( (config->maxcycle > 0 && n < config->maxcycle) || config->maxcycle == 0)
	{
		printf("Server is Listening on %d ..\n", config->port);

		request = getRequest(config->port);

		if ( request == NULL)
		{	
			printf("Request Error : EXIT !\n");
			exit(EXIT_FAILURE);
		}

		printf("-> Request received from client %d\n", request->clientId);
		printf("Client [%d] [%s:%d]\n",request->clientId,inet_ntoa(request->clientAddress->sin_addr),htons(request->clientAddress->sin_port));
		printf("Request content :\n%.*s\n\n",request->len,request->buf);  

		answer = get_answer(request, n);
		if( answer ) {
			writeDirectClient(answer->clientId, answer->buf, answer->len);
			endWriteDirectClient(answer->clientId);
		}

		requestShutdownSocket(answer->clientId);

		freeRequest(request);
		free(answer);
		n++;
	}

	free_config(config);
	printf("Server is closing ..\n");
	return 0;
}

message *get_answer(message *request, int index){
	message *answer = malloc(sizeof(message));

	if (answer){
		int res_parser = parser(request->buf, request->len);

		if ( M_PRINT_TREE ) {
			char name[50];
			sprintf(name,"output_request_n_%d.log",index);
			FILE *file = open_file(name,"w");
			print_tree(file, getRootTree());
			fclose(file);
		}

		if (res_parser ) {
			answer->buf= ok_message;
			answer->len= strlen(ok_message); 
		}
		else {
			answer->buf= error_message;
			answer->len= strlen(error_message); 
		}
		answer->clientId = request->clientId;

		purgeTree(getRootTree());
	}
	return answer;
}







// ---- PREVIOUS MAIN ---- //

// int main(int argc,char *argv[])
// {
// 	int res,fi;
// 	char *p=NULL,*addr;
//     struct stat st;

// 	if (argc < 3 ) { printf("Usage: httpparser <file> <search>\nAttention <search> is case sensitive\n"); exit(1); }
// 	/* ouverture du fichier contenant la requête */ 
// 	if ((fi=open(argv[1],O_RDWR)) == -1) {
//                 perror("open");
//                 return false;
//         }
//         if (fstat(fi, &st) == -1)           /* To obtain file size */
//                 return false;
//         if ((addr=mmap(NULL,st.st_size,PROT_WRITE,MAP_PRIVATE, fi, 0)) == NULL )
//                 return false;

// 	// This is a special HACK since identificateur in C can't have character '-'
	
// 	if (argc == 3 ) { 
// 		p=argv[2]; 	
// 		printf("searching for %s\n",p); 
// 	}
// 	// call parser and get results. 
// 	if (res=parser(addr,st.st_size)) {
// 		printf("Sucess !\n");
// 		_Token *r,*tok; 
// 		void *root=NULL;
// 		root=getRootTree(); 
// 		r=searchTree(root,p); 
// 		tok=r; 
// 		while (tok) {
// 			int l; 
// 			char *s; 
// 			s=getElementValue(tok->node,&l); 
// 			printf("FOUND [%.*s]\n",l,s);
// 			tok=tok->next; 
// 		}
// 		purgeElement(&r);
// 		purgeTree(root);
// 	}

// 	if ( res == 0)
// 	{
// 		printf("Error on : %s\n", argv[1]);
// 	}
	
// 	close(fi);
// 	return(res); 
// }
