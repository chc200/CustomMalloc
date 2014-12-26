malloctest: mymalloc.o mallocdrv.o
	gcc -o $@ $^
mymalloc.o: mymalloc.c mymalloc.h 
	gcc -c mymalloc.c 
testDriver.o: mallocdrv.c mymalloc.h 
	gcc -c mallocdrv.c 
clean: 
	rm -f *.o malloctest
