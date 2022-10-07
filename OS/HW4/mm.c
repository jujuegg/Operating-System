#include "mm.h"

static Header base; /* empty list to get started */
static Header *freep = NULL; /* start of free list */

/* morecore: ask system for more memory */
static Header *morecore(size_t nu)
{
	char *cp;
	Header *up;
	
	if (nu < NALLOC)
		nu = NALLOC;
	cp = sbrk(nu * sizeof(Header));
	if (cp == (char *) -1)	/* no space at all */
		return NULL;
	up = (Header *) cp;		/* make up equal to cp and set size */
	up->s.size = nu;
	myfree((void *)(up+1));		/* put up into free list */
	
	return freep;
}
/* mymalloc: general-purpose storage allocator */
void *mymalloc(size_t size)		
{
	/* p: current pointer of free list, prevp: prev pointer of free list */
	Header *p, *prevp;
	Header *morecore(size_t);
	size_t nunits; 	/* a number of Header we need to get */
	
	/* round up with integer division */
	nunits = (size + sizeof(Header)-1) / sizeof(Header) + 1; 
	if ((prevp = freep) == NULL)	/* no free list yet */
	{ 
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	for (p = prevp->s.ptr ; ; prevp = p, p = p->s.ptr)
	{
		if(p->s.size >= nunits)		/* big enough */
		{
			if (p->s.size == nunits)	/* exactly */
				prevp->s.ptr = p->s.ptr;
			else	/* allocate tail end */
			{
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			return (void *)(p+1);
		}
		if (p == freep)		/* wrapped around free list */
			if ((p = morecore(nunits)) == NULL)		/* none left */
				return NULL;
	}
}
/* free: put block ap in free list */
void myfree(void *ap)
{
	Header *bp, *p;

	bp = (Header *)ap - 1;		/* point to block header */
	for (p = freep ; !(bp > p && bp < p->s.ptr) ; p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;		/* freed block at start or end of arena */
	
	if (bp + bp->s.size == p->s.ptr)	/* join to upper nbr */
	{ 
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	}
	else
		bp->s.ptr = p->s.ptr;
	
	if (p + p->s.size == bp)	/* join to lower nbr */
	{
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}
	else
		p->s.ptr = bp;
	freep = p;
}
void *myrealloc(void *ptr, size_t size)
{
	Header *bp;
	size_t nunits;

	if(ptr == NULL)
		return mymalloc(size);

	bp = (Header *)ptr - 1;
	nunits = (size + sizeof(Header) - 1) / sizeof(Header) + 1;

	if(bp->s.size >= nunits)
		return ptr;

	void *np = mymalloc(size);
	if(np == NULL)
		return NULL;

	memcpy(np, ptr, (bp->s.size - 1) * sizeof(Header));
	myfree(ptr);
	return np;
}
void *mycalloc(size_t nmemb, size_t size)
{
	size_t all = nmemb * size;
	void *np = mymalloc(all);
	memset(np,0,sizeof(np));
	if(np == NULL)
		return NULL;
	return np;
}
