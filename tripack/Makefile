
#STARBASE=/Users/john/src/starbase
STARBASE=/data/mmti/src/starbase

#FC=gfortran-mp-4.7 -m32 -g
#FC=gcc -m32 -g
FC=f77 -m32

CFLAGS= -m32 -I$(STARBASE)/include -DSTDC_HEADERS -g
LDFLAGS= -L$(STARBASE)/tablelib -L$(STARBASE)/utillib -ltable -lutil -lm

all: src.f tripack.f fitztable


src.f : src.f.orig
	./fproc < src.f.orig > src.f

tripack.f : tripack.f.orig
	./fproc < tripack.f.orig > tripack.f

fitztable : fitztable.c src.o tripack.o
	$(CC) $(CFLAGS) -o fitztable fitztable.c src.o tripack.o $(LDFLAGS)

clean:
	rm -f *.f *.o
