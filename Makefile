

OS  =$(shell uname)
ARCH=$(OS).$(shell uname -m)


acorn.Linux.x86_64  : BITS = -m64
acorn.Linux.i386    : BITS = -m32
acorn.Darwin.x86_64 : BITS = -m64
acorn.Darwin.i386   : BITS = -m32

INC= -I/Users/john/include -I/home/john/include

CFLAGS=$(INC) -msse -fPIC

SRC	= acorn.cpp aperture.cpp glass/glass.c glass/acorn-glass.cpp tpool/tpool.c
TCL	= acorn.tcl acorn-model.tcl uda.tcl agf.tcl zmx.tcl func.tcl


SURFS	= surfaces/lib/$(ARCH)/simple.so		\
	  surfaces/lib/$(ARCH)/zernike.so		\
	  surfaces/lib/$(ARCH)/lens-array-rect.so	\
	  surfaces/lib/$(ARCH)/lens-array-hex.so	\
	  surfaces/lib/$(ARCH)/evenasph.so		\
	  surfaces/lib/$(ARCH)/dgrating.so

ACORN_SRCS = $(SRC) $(TCL)
ACORN_OBJS = $(SURFS) 
ACORN_UTIL = surfaces/acorn-utils.h

all: zernike acorn.$(OS) arec.$(OS) $(ACORN_OBJS)

acorn.Darwin : acorn.Darwin.i386 acorn.Darwin.x86_64 acorn.Darwin.i386 acorn.Darwin.x86_64 test.Darwin
acorn.Linux  :                   acorn.Linux.x86_64                    acorn.Linux.x86_64  test.Linux

zernike/zernike.c :
	cd zernike; $(MAKE)


acorn.Darwin : acorn.Darwin.i386 acorn.Darwin.x86_64

acorn.Darwin.i386   : lib/acorn/macosx-ix86/acorn.dylib		arec.Darwin.i386
acorn.Darwin.x86_64 : lib/acorn/macosx-x86_64/acorn.dylib	arec.Darwin.x86_64
acorn.Linux.x86_64  : lib/acorn/linux-x86_64/acorn.so		arec.Linux.x86_64

lib/acorn/macosx-ix86/acorn.dylib   :	$(ACORN_SRCS)	$(ACORN_OBJS)
	ARCH=$(ARCH) critcl -target macosx-x86_32 -pkg acorn 
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86

lib/acorn/macosx-x86_64/acorn.dylib :	$(ACORN_SRCS)	$(ACORN_OBJS)
	ARCH=$(ARCH) critcl -target macosx-x86_64 -pkg acorn 

lib/acorn/linux-x86_64/acorn.so :	$(ACORN_SRCS)	$(ACORN_OBJS)
	ARCH=$(ARCH) critcl -pkg acorn 



surfaces/lib/$(ARCH)/simple.so : surfaces/simple.cpp acorn.h $(ACORN_UTIL)
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/simple.cpp -o surfaces/lib/$(ARCH)/simple.so

surfaces/lib/$(ARCH)/evenasph.so : surfaces/evenasph.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/evenasph.cpp -o surfaces/lib/$(ARCH)/evenasph.so

surfaces/lib/$(ARCH)/dgrating.so : surfaces/dgrating.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/dgrating.cpp -o surfaces/lib/$(ARCH)/dgrating.so

surfaces/lib/$(ARCH)/zernike.so : surfaces/zernike.cpp acorn.h zernike/lib/$(ARCH)/zernike.o
	mkdir -p zernike/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/zernike.cpp zernike/lib/$(ARCH)/zernike.o -o surfaces/lib/$(ARCH)/zernike.so
	
surfaces/lib/$(ARCH)/lens-array-rect.so : surfaces/lens-array-rect.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/lens-array-rect.cpp -o surfaces/lib/$(ARCH)/lens-array-rect.so
	
surfaces/lib/$(ARCH)/lens-array-hex.so : surfaces/lens-array-hex.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/lens-array-hex.cpp -o surfaces/lib/$(ARCH)/lens-array-hex.so

tpool/lib/$(ARCH)/tpool.o : tpool/tpool.c
	mkdir -p tpool/lib/$(ARCH)
	gcc $(BITS) -fPIC -O2 -c $(INC) tpool/tpool.c -o tpool/lib/$(ARCH)/tpool.o

glass/lib/$(ARCH)/glass.o : glass/glass.c
	mkdir -p glass/lib/$(ARCH)
	gcc $(BITS) -fPIC -O2 -c $(INC) glass/glass.c -o glass/lib/$(ARCH)/glass.o

zernike/lib/$(ARCH)/zernike.o : zernike/zernike.c 
	mkdir -p zernike/lib/$(ARCH)
	gcc $(BITS) -fPIC -O2 -c zernike/zernike.c -o zernike/lib/$(ARCH)/zernike.o

arec.Darwin.i386	:
	cd arec; $(MAKE) arec.Darwin.i386

arec.Darwin.x86_64	:
	cd arec; $(MAKE) arec.Darwin.x86_64

arec.Linux.x86_64 	:
	cd arec; $(MAKE) arec.Linux.x86_64

clean:
	rm -f *.o */*.o


