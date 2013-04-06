

INC= -I/Users/john/include -I/home/john/include
BITS=-m32 -msse3
#BITS=-m64 -fPIC

arec	= lib/arec/pkgIndex.tcl
acorn	= lib/acorn/pkgIndex.tcl

OBJ	= acorn.o glass/glass.o glass/acorn-glass.o tpool/tpool.o
TCL	= acorn.tcl acorn-model.tcl uda.tcl agf.tcl zmx.tcl func.tcl

SURFS	= surfaces/simple.so		\
	  surfaces/zernike.so		\
	  surfaces/lens-array-rect.so	\
	  surfaces/lens-array-hex.so	\
	  surfaces/evenasph.so		\
	  surfaces/dgrating.so

ACORN_UTIL 	= surfaces/acorn-utils.h

all : $(acorn) $(SURFS)

$(acorn) : acorn.h $(arec) $(OBJ) $(TCL)
	#critcl31 -force -pkg acorn
	#
	critcl -target macosx-x86_32 -force -pkg acorn
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86

$(arec) : arec.c arec.h arec.tcl
	#critcl31 -force -pkg arec
	critcl -target macosx-x86_32 -force -pkg arec
	rm -rf lib/arec/macosx-ix86
	mv lib/arec/macosx-x86_32 lib/arec/macosx-ix86

acorn.o : acorn.cpp acorn.h
	g++ $(BITS) -c $(INC) acorn.cpp -o acorn.o

surfaces/aperture.o : surfaces/aperture.cpp acorn.h
	g++ $(BITS) -03 -c $(INC) surfaces/aperture.cpp -o surfaces/aperture.o

surfaces/simple.so : surfaces/simple.cpp acorn.h $(ACORN_UTIL)
	g++ $(BITS) -O3 -shared $(INC) surfaces/simple.cpp surfaces/aperture.o -o surfaces/simple.so

surfaces/evenasph.so : surfaces/evenasph.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/evenasph.cpp surfaces/aperture.o -o surfaces/evenasph.so

surfaces/dgrating.so : surfaces/dgrating.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/dgrating.cpp -o surfaces/dgrating.so

surfaces/zernike.so : surfaces/zernike.cpp acorn.h zernike/zernike.o
	g++ $(BITS) -O3 -shared $(INC) surfaces/zernike.cpp surfaces/aperture.o zernike/zernike.o -o surfaces/zernike.so
	
surfaces/lens-array-rect.so : surfaces/lens-array-rect.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/lens-array-rect.cpp -o surfaces/lens-array-rect.so
	
surfaces/lens-array-hex.so : surfaces/lens-array-hex.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/lens-array-hex.cpp -o surfaces/lens-array-hex.so

glass/glass.o : glass/glass.c
	gcc $(BITS) -O3 -c $(INC) glass/glass.c -o glass/glass.o

glass/acorn-glass.o : glass/acorn-glass.c glass/glass.h
	gcc $(BITS) -O3 -c $(INC) glass/acorn-glass.c -o glass/acorn-glass.o

zernike/zernike.o : zernike/zernike.c 
	gcc $(BITS) -O3 -c zernike/zernike.c -o zernike/zernike.o

clean:
	rm -f acorn.o


