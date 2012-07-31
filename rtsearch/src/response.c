#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "response.h"

struct response *response_new(int argc,STATUS status)
{
	struct response *res;
	res=calloc(1,sizeof(struct response));
	res->argv=calloc(argc,sizeof(char*));
	res->argc=argc;
	res->status=status;
	return res;
}


void response_detch(struct response *res,char *ackbuf)
{
	int i;
	switch(res->status){
		case OK:
			strcat(ackbuf,"+OK\r\n");
			break;

		case OK_200:
			break;

		case OK_404:
			strcat(ackbuf,"$-1\r\n");
			return;

		case OK_PONG:
			strcat(ackbuf,"+PONG\r\n");
			return;	
		
		case ERR:
			strcat(ackbuf,"-OK\r\n");
			return;;
	}

	for(i=0;i<res->argc;i++){
		char ls[MAX_RES_LEN]={0};
		int l=strlen(res->argv[i]);
		sprintf(ls,"$%d\r\n",l);
		strcat(ackbuf,ls);

		strcat(ackbuf,res->argv[i]);
		strcat(ackbuf,"\r\n");
	}
}

void response_dump(struct response *res)
{
	int i;
	if(!res)
		return;

	printf("response-dump--->");
	printf("argc:<%d>\n",res->argc);
	printf("cmd:<%d>\n",res->status);
	for(i=0;i<res->argc;i++){
		printf("		argv[%d]:<%s>\n",i,res->argv[i]);
	}
	printf("\n");
}

void response_free(struct response *res)
{
    if(res){
        free(res->argv);
        free(res);
    }
}



