
#TCL_LIBS=/home/john/lib 
TCL_LIBS=/Users/john/lib 

test : test.c tpool.o tpool.h
	$(CC) -m32 -DTCL_STUBS -DTCL_THREADS=1 -I./tcl/generic -g -o test test.c tpool.o -L$(TCL_LIBS) -ltclstub8.6

tpool.o : tpool.c tpool.h
	$(CC) -m32 -DTCL_THREADS=1 -I./tcl/generic -c -o tpool.o tpool.c

clean:
	rm -f *.o
	rm -f test

