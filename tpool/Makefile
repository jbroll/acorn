
TCL_LIBS=/home/john/lib 
#TCL_LIBS=/Users/john/lib 

CFLAGS = -g -DTCL_THREADS=1 

test : test.c tpool.o tpool.h
	$(CC) $(CFLAGS) -I./tcl/generic -g -o test test.c tpool.o -L$(TCL_LIBS) -ltcl8.6

tpool.o : tpool.c tpool.h
	$(CC) $(CFLAGS) -I./tcl/generic -c -o tpool.o tpool.c

clean:
	rm -f *.o
	rm -f test

