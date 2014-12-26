#include "mymalloc.h"

int main()
{	
	void *a=my_buddy_malloc(110);
	void *b=my_buddy_malloc(70);
	void *c=my_buddy_malloc(70);
	void *d=my_buddy_malloc(499);

	my_free(a);	
	my_free(b);
	my_free(c);
	my_free(d);
}
