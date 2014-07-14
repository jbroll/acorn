

#FC=gfortran-mp-4.7 -m32 -g
#FC=gfortran
#FC=gcc -m32 -g
#FC=f77 -m32

FFLAGS= -fPIC
CFLAGS= -fPIC

tripack	= lib/tripack/pkgIndex.tcl

all : $(tripack)

$(tripack) : tripack.tcl tripack.o
	#critcl -target macosx-x86_32 -force -pkg tripack
	../critcl31.kit -force -pkg tripack
	#rm -rf lib/tripack/macosx-ix86
	#mv lib/tripack/macosx-x86_32 lib/tripack/macosx-ix86
