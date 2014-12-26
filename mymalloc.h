#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#define MAX_MEM 1<<30
#include <stdio.h>

typedef struct Node{
         unsigned char header;
         struct Node* previous;
         struct Node* next;
}node;


void *divider(int index, int baseCase);
int power(int num);

void *findFreeSpace(int size);

void *my_buddy_malloc(int size);
void my_free(void *ptr);

