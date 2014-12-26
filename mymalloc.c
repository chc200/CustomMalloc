#include "mymalloc.h"

node *fList[26] = {NULL};
static void *base = NULL;

/*divider takes a node larger than the size we need and recursively divides it until
 *it is the correct size.  It adds these two nodes into the correct size in our freeArray
 *and returns a pointer to the 1st of the two nodes.
 *
 */
void *divider(int index, int baseCase) {
		
	node *temporary = fList[index + 1]; // get large potion of memory to divide
	

	int size = ((1 << (index + 6)) / 2);//calculates half of this large chunck of memory
	node *toSplit =(char *)temporary + size; // adds to our curernt position to get the pointer directly in the middle
        //printf("index=%d, size=%d\n", index, size);

	if(temporary->next != NULL) {
		fList[index+1] = temporary->next;
		temporary->next= NULL;
		fList[index+1]->previous = NULL;
	}
	else{
		fList[index+1]=NULL;
	}	
	//checking to see if the index we are removing these nodes from have something it it so we don't 
	//lose the other nodes in the fList

	temporary->next = toSplit;
	toSplit->previous = temporary;
	//creating 2 independent nodes temporary and toSplit

	if(fList[index] != NULL){
		toSplit->next = fList[index];
		fList[index]->previous=toSplit;
	}
	else{
		toSplit->next = NULL;
	}
	//checking to see if index we are placing these nodes in have a node already in them


	fList[index] = temporary;//places our 2 nodes in our fList
	temporary->previous=NULL;//sets the previous to our first node in our fList to NULL

	temporary->header = index+5;
	toSplit->header = index+5;
	//resetting the headers to the correct size

	if(fList[index]->header == baseCase+5){
		fList[index]->header |=128;
		void *tmp = fList[index];
                fList[index]=fList[index]->next;
		if(fList[index]!=NULL)
		{
			fList[index]->previous=NULL;	
		}
		return tmp;
	}
	else{
		divider(index-1,baseCase);
	}
	//recursion base case.
}
/*
 *power function finds what index is large enough to 
 *store our malloced size.  It does it by bit shifting 1 to 
 * increment by the powers of 2.
 */
int power(int num)
{
	int i;
	for(i=5; i<=30; i++)
	{
		if((1<<i) >num)
			{
				return i;
			}
	}
}

/*findFreeSpace itterates through our array starting from the best possible fit for our size
 *if that size exists we will return the pointer to that node and remove it from our array
 *otherwise we will need to call our divider functions to create a better fit for the space
 *we are trying to allocate.
 */
void *findFreeSpace(int size){
	int i;
	for(i=size-5; i<26; i++)//accounting for the -5 we can just return -5 then dont need to sub
		{
			if(fList[i] != NULL)
			{	
				if(fList[i]->header==size)// added +5
				{
					fList[i]->header |=128;//setting the first bit to 1 --> used
					void *temporaryPnt= fList[i];
					fList[i]=fList[i]->next;
					if(fList[i]!=NULL)
					{
						fList[i]->previous=NULL;
					}
					return temporaryPnt;
				} 
				//case if we have a node of the correct size
				else
				{ 
					void *pointer = divider(i-1, size-5);
					return pointer;
				}
				//case if we need to make a node of the correct size
			}
		}
		printf("We don't have any more memory sorry");
// if we get here we dont have enough memory to allocate anymore
}

void *my_buddy_malloc(int size)
{
    if(base == NULL)
    	{
            base = mmap(NULL, MAX_MEM, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
       		fList[25]=base;
			fList[25] -> header = 30;
			fList[25]->next=NULL;
			fList[25]->previous = NULL;
 		}

    assert(base!=NULL);

	int thePower = power(size);
	void *space = findFreeSpace(thePower);       
	return space;
}


void my_free(void *ptr)
{
	node *free = (struct Node*)ptr;//casting our void pointer as a struct node

	if(free->header ==30 )
	{
		return;
	}
	//recursion base case

	free->header &=127;//setting 1st bit as 0 -->freed
	
        void *val = ((ptr-base) ^ (1 << free->header)) + base;
        node *findBuddy = (struct Node*)val;
	unsigned char myBits= findBuddy->header;
	myBits=myBits>>7;
	//calculation for buddy to see if our buddy is available or not

	if(myBits == 0 ) 
	{
		//if we get here our buddy is free!
		if(findBuddy->next==NULL && findBuddy->previous==NULL)
		{
				fList[findBuddy->header-5]=NULL;
		}
		else if(findBuddy->next == NULL && findBuddy->previous != NULL)
		{
				node *supertemporary = findBuddy->previous;
				supertemporary->next = NULL;
				fList[findBuddy->header-5]=NULL;
		}
		else if(findBuddy->next!= NULL && findBuddy->previous != NULL)
		{
				node *supertemporaryPrev = findBuddy->previous;
				node *supertemporaryNext = findBuddy->next;		
				supertemporaryPrev->next = supertemporaryNext;
				supertemporaryNext->previous = supertemporaryPrev;
				fList[findBuddy->header-5]=NULL;
		}
		else if(findBuddy->next!=NULL && findBuddy->previous == NULL)
		{
				
				fList[findBuddy->header-5] = findBuddy->next;
				fList[findBuddy->header-5]->previous = NULL;
				findBuddy->next = NULL;
		}
		//deletion of buddy from our fList (so we can move it up to the next index)
		
		int greater = val-ptr;//calculation to see what node is the front node
		if(greater>0)
		{
			//ptr
			free->header +=1;	
			if(fList[free->header-5]!=NULL)
			{
				fList[free->header-5]->previous=free;
				free->next=fList[free->header-5];
				fList[free->header-5]=free;
				free->previous=NULL;
				my_free(ptr);		
			}
			else
			{
				fList[free->header-5]=free;	
				free->previous= NULL;
				free->next = NULL;
			}
		}
		//colasing the nodes together and moving them up an index
		else
		{
			//val
			findBuddy->header +=1;
			if(fList[findBuddy->header-5]!=NULL)
			{
				fList[findBuddy->header-5]->previous=findBuddy;
				findBuddy->next=fList[findBuddy->header-5];
				fList[findBuddy->header-5]=findBuddy;
				findBuddy->previous= NULL;
				my_free(val);

			}
			else
			{
				fList[findBuddy->header-5]=findBuddy;	
				findBuddy->previous= NULL;
				findBuddy->next= NULL;
			}
		}	
		//colasing the nodes together and moving them up and index
	}
	else
	{
		//our buddy didnt exist :(
		if(fList[free->header-5]!=NULL)
		{
			free->next=fList[free->header-5];
			fList[free->header-5]->previous = free;
			fList[free->header-5]=free;
			free->previous = NULL;
		}
		else
		{
			free->next=NULL;
			free->previous = NULL;
			fList[free->header-5]=free;
		}
		//check to see if the place we are putting our node has another node
	}

}

