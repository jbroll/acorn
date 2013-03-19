

INC= -I/Users/john/include -I/home/john/include
BITS=-m32 -msse3
#BITS=-m64 -fPIC

arec	= lib/arec/pkgIndex.tcl
acorn	= lib/acorn/pkgIndex.tcl

OBJ	= acorn.o aperture.o glass/glass.o glass/acorn-glass.o tpool/tpool.o
TCL	= acorn.tcl uda.tcl agf.tcl zmx.tcl func.tcl

SURFS	= surfaces/simple.so surfaces/zernike.so surfaces/lens-array.so

all : $(acorn) $(SURFS)

$(acorn) : acorn.h $(arec) $(OBJ) $(TCL)
	#critcl31 -force -pkg acorn
	#
	critcl31-issue21 -target macosx-x86_32 -force -pkg acorn
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86

$(arec) : arec.c arec.h arec.tcl
	#critcl31 -force -pkg arec
	critcl31 -target macosx-x86_32 -force -pkg arec
	rm -rf lib/arec/macosx-ix86
	mv lib/arec/macosx-x86_32 lib/arec/macosx-ix86

acorn.o : acorn.cpp acorn.h
	g++ $(BITS) -c $(INC) acorn.cpp -o acorn.o

aperture.o : aperture.cpp acorn.h
	g++ $(BITS) -03 -c $(INC) aperture.cpp -o aperture.o

surfaces/simple.so : surfaces/simple.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/simple.cpp -o surfaces/simple.so

surfaces/zernike.so : surfaces/zernike.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/zernike.cpp zernike/zernike.o -o surfaces/zernike.so
	
surfaces/lens-array.so : surfaces/lens-array.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/lens-array.cpp -o surfaces/lens-array.so
	
glass/glass.o : glass/glass.c
	gcc $(BITS) -O3 -c $(INC) glass/glass.c -o glass/glass.o

glass/acorn-glass.o : glass/acorn-glass.c glass/glass.h
	gcc $(BITS) -O3 -c $(INC) glass/acorn-glass.c -o glass/acorn-glass.o

clean:
	rm -f acorn.o


