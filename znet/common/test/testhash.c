#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

int main(void)
{
	hash_t *hash = hash_make();
	char *key = NULL;
	int n = 100,*val;
	unsigned int count;

	hash_index_t *hi;
	unsigned int sum = 0;
	int i;

	for(i = 0;i<100000;++i)
	{
		key = malloc(sizeof(char) * 256);
		memset(key,0,256);

		val = malloc(sizeof(int));
		*val = i;

		sprintf(key,"char%10d",i);
		hash_set(hash,key,HASH_KEY_STRING,val);

		void *entry_key;
		val = hash_get(hash,key,HASH_KEY_STRING,&entry_key);
		if(entry_key)
		{
		    printf("key:%s\n",(char*)entry_key);
		}

	//	if(val)
	//	printf("val:%d\n",*val);
	}

	for (hi = hash_first(hash); hi ; hi = hash_next(hi)){
		hash_this(hi,(const void **)&key,NULL,(void **)&val);
		hash_set(hash,key,HASH_KEY_STRING,NULL);
		printf("val:%d\n",*(int *)val);
		sum += *(int *)val;
		free(key);
		free(val);
	}
	
	printf("sum:%d\n",sum);

	count = hash_count(hash);
	printf("count:%u\n",count);

	hash_clear(hash);

	count = hash_count(hash);
	printf("count after clear:%u\n",count);

	hash_destroy(hash);

	return 0;
}
