#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "request.h"

/* useful symbols:
 * '*' and '$' states are:3
 * '0' '1' '2' '3' '4' '5' '6' '7' '8' '9';states are 2
 */
//==============================DFA states=================================//
unsigned char _table[256]={
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0,       0,       3,       0,       0,       3,       0,       0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0,       0,       0,       0,       3,       0,       0,       0,
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        0,       0,       3,       0,       0,       0,       0,       0,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
        2,       2,       2,       2,       2,       2,       2,       2,
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
        2,       2,       0,       0,       0,       0,       0,       0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        0,       0,       0,       0,       0,       1,       0,       1,
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
        0,       0,       0,       0,       1,       0,       0,       0,
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
        0,       0,       0,       0,       0,       0,       0,       0 
	};


struct request *request_new(char *querybuf)
{
	struct request *req;
	req=calloc(1,sizeof(struct request));
	req->querybuf=querybuf;
	return req;
}


int req_state_len(struct request *req,char *sb)
{
	int term=0,first=0;
	char c;
	int i=req->pos;
	int pos=i;
	while((c=req->querybuf[i])!='\0'){
		first++;
		pos++;
		switch(c){
			case '\r':
				term=1;
				break;
				  
			case '\n':
				  if(term){
					req->pos=pos;
					return STATE_CONTINUE;
				}
				else
					return STATE_FAIL;
			default:
				if(first==1){
					/* the first symbol is not '*' or '$'*/
					if(_table[(unsigned char)c]!=3)
						return STATE_FAIL;
				}else{
					/* the symbol must be numeral*/
					if(_table[(unsigned char)c]==2){
						*sb=c;
						sb++;
					}
					else
						return STATE_FAIL;
				}
				break;
		}
		i++;
	}

	return STATE_FAIL;
}


int request_parse(struct request *req)
{
	int  i;
	char sb[BUF_SIZE]={0};

	if(req_state_len(req,sb)!=STATE_CONTINUE){
		fprintf(stderr,"argc format ***ERROR***,packet:%s",sb);
		return 0;
	}
	req->argc=atoi(sb);

	req->argv=(char**)calloc(req->argc,sizeof(char*));
	for(i=0;i<req->argc;i++){
		int argv_len;
		char *v;

		/*parse argv len*/
		memset(sb,0,BUF_SIZE);
		if(req_state_len(req,sb)!=STATE_CONTINUE){
			fprintf(stderr,"argv's length format ***ERROR***,packet:%s",sb);
			return 0;
		}
		argv_len=atoi(sb);

		/*get argv*/
		v=(char*)calloc(argv_len,sizeof(char));
		memcpy(v,req->querybuf+(req->pos),argv_len);
		req->argv[i]=v;	
		req->pos+=(argv_len+2);

		if(i==0){
			int k;
			for(k=0;k<argv_len;k++){
				if(v[k]>='A' && v[k]<='Z')
					v[k]+=32;
			}

			if(strcmp(v,"ping")==0){
				req->cmd=CMD_PING;
			}else if(strcmp(v,"get")==0){
				req->cmd=CMD_GET;
			}else if(strcmp(v,"set")==0){
				req->cmd=CMD_SET;
			}else if(strcmp(v,"del")==0){
				req->cmd=CMD_DEL;
			}else{
				req->cmd=CMD_UNKNOW;
			}
		}
	}
	return 1;
}

void request_dump(struct request *req)
{
	int i;
	if(!req)
		return;

	printf("request-dump--->");
	printf("argc:<%d>\n",req->argc);
	printf("cmd:<%d>\n",req->cmd);
	for(i=0;i<req->argc;i++){
		printf("		argv[%d]:<%s>\n",i,req->argv[i]);
	}
	printf("\n");
}

void request_free(struct request *req)
{
	int i;
	if(req){
		for(i=0;i<req->argc;i++){
			if(req->argv[i])
				free(req->argv[i]);
		}
		free(req->argv);
		free(req);
	}
}
