

INC= -I/Users/john/include -I/home/john/include
BITS=-m32
#BITS=-m64 -fPIC

arec	= lib/arec/pkgIndex.tcl
acorn	= lib/acorn/pkgIndex.tcl

OBJ	= acorn.o aperture.o
TCL	= acorn.tcl uda.tcl

SURFS	= surfaces/simple.so surfaces/zernike.so

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
	g++ $(BITS) -c $(INC) aperture.cpp -o aperture.o

surfaces/simple.so : surfaces/simple.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/simple.cpp -o surfaces/simple.so

surfaces/zernike.so : surfaces/zernike.cpp acorn.h
	g++ $(BITS) -O3 -shared $(INC) surfaces/zernike.cpp zernike/zernike.o -o surfaces/zernike.so

clean:
	rm -f acorn.o


