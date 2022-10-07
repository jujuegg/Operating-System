#include "mm.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p = mymalloc(100);
	printf("test mymalloc:\naddress : %p\n",p);
	printf("value : %d\n",*p);
	myfree(p);

	int *q = mycalloc(1000,sizeof(int));
	printf("test mycalloc:\naddress : %p\n",q+1000);
	printf("value : %d\n",*q);

	int *q2 = myrealloc(q,sizeof(int)*1000*2);
	printf("test myrealloc:\naddress : %p\n",q+2000);
	printf("value : %d\n",*q);
	
	myfree(q2);
	
	return 0;
}
