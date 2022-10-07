#ifndef __MY_MM_H_INCLUDED__
#define __MY_MM_H_INCLUDED__
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#define NALLOC 1024 /* minimum units to request */
typedef long Align; /* for alignment to long boundary */

union header	/* block header */
{ 
	struct
	{
		union header *ptr; /* next block if on free list */
		size_t size; /* size of this block */
	} s;
	Align x; /* force alignment of blocks, size of union is same as long */
};
typedef union header Header;

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void *mycalloc(size_t nmemb, size_t size);

#endif
