

OS  =$(shell uname)
ARCH=$(OS).$(shell uname -m)

include $(CONFIG)


INC= -I/Users/john/include -I/home/john/include
#BITS=-m32
#BITS=-m64 -fPIC

CFLAGS=$(INC) -msse -fPIC

arec	= lib/arec/pkgIndex.tcl
acorn	= lib/acorn/pkgIndex.tcl

OBJ	= tpool/lib/$(ARCH)/tpool.o glass/lib/$(ARCH)/glass.o glass/lib/$(ARCH)/acorn-glass.o
TCL	= acorn.tcl acorn-model.tcl uda.tcl agf.tcl zmx.tcl func.tcl

SURFS	= surfaces/lib/$(ARCH)/simple.so		\
	  surfaces/lib/$(ARCH)/zernike.so		\
	  surfaces/lib/$(ARCH)/lens-array-rect.so	\
	  surfaces/lib/$(ARCH)/lens-array-hex.so	\
	  surfaces/lib/$(ARCH)/evenasph.so		\
	  surfaces/lib/$(ARCH)/dgrating.so

ACORN_OBJS = $(SURFS) $(OBJ)

ACORN_UTIL 	= surfaces/acorn-utils.h

all: zernike acorn.$(OS) arec.$(OS) $(SURFS)

acorn.Darwin : acorn.Darwin.i386 acorn.Darwin.x86_64 acorn.Darwin.i386 acorn.Darwin.x86_64 test.Darwin
acorn.Linux  :                   acorn.Linux.x86_64                    acorn.Linux.x86_64  test.Linux

zernike/zernike.c :
	cd zernike; $(MAKE)

arec.Darwin.i386	:
	cd arec; $(MAKE) arec.Darwin.i386

arec.Darwin.x86_64	:
	cd arec; $(MAKE) arec.Darwin.x86_64

arec.Linux.x86_64 	:
	cd arec; $(MAKE) arec.Linux.x86_64


acorn.Darwin : acorn.Darwin.i386 acorn.Darwin.x86_64

acorn.Darwin.i386   : lib/acorn/macosx-ix86/acorn.dylib
acorn.Darwin.x86_64 : lib/acorn/macosx-x86_64/acorn.dylib

lib/acorn/macosx-ix86/acorn.dylib   : $(ACORN_OBJS)
	ARCH=$(ARCH) critcl -target macosx-x86_32 -pkg acorn 
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86
	rm opcodes.o

lib/acorn/macosx-x86_64/acorn.dylib : $(ACORN_OBJS)
	ARCH=$(ARCH) critcl -target macosx-x86_64 -pkg acorn 
	rm opcodes.o

$(arec) : arec.c arec.h arec.tcl
	#critcl31 -force -pkg arec
	critcl $(OS_CRITFLAGS) -force -pkg arec
	#rm -rf lib/arec/macosx-ix86
	#mv lib/arec/macosx-x86_32 lib/arec/macosx-ix86

#acorn.o : acorn.cpp acorn.h
#	g++ $(BITS) -O2 -c $(INC) acorn.cpp -o acorn.o

#surfaces/aperture.o : surfaces/aperture.cpp acorn.h
#	g++ $(BITS) -O2 -c $(INC) surfaces/aperture.cpp -o surfaces/aperture.o

surfaces/lib/$(ARCH)/simple.so : surfaces/simple.cpp acorn.h $(ACORN_UTIL)
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/simple.cpp -o surfaces/lib/$(ARCH)/simple.so

surfaces/lib/$(ARCH)/evenasph.so : surfaces/evenasph.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/evenasph.cpp -o surfaces/lib/$(ARCH)/evenasph.so

surfaces/lib/$(ARCH)/dgrating.so : surfaces/dgrating.cpp acorn.h
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/dgrating.cpp -o surfaces/lib/$(ARCH)/dgrating.so

surfaces/lib/$(ARCH)/zernike.so : surfaces/zernike.cpp acorn.h zernike/zernike.o
	mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/zernike.cpp zernike/zernike.o -o surfaces/lib/$(ARCH)/zernike.so
	
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

glass/lib/$(ARCH)/acorn-glass.o : glass/acorn-glass.c glass/glass.h
	gcc $(BITS) -fPIC -O2 -c $(INC) glass/acorn-glass.c -o glass/lib/$(ARCH)/acorn-glass.o

zernike/lib/$(ARCH)/zernike.o : zernike/zernike.c 
	gcc $(BITS) -fPIC -O2 -c zernike/zernike.c -o zernike/zernike.o

clean:
	rm -f *.o */*.o


