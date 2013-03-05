

FC=gfortran-mp-4.7 -m32 -g
#FC=gcc -m32 -g
#FC=f77 -m32

CFLAGS= -m32

tripack	= lib/tripack/pkgIndex.tcl

all : $(tripack)

$(tripack) : tripack.tcl tripack.o
	critcl31-issue21 -target macosx-x86_32 -force -pkg tripack
	rm -rf lib/tripack/macosx-ix86
	mv lib/tripack/macosx-x86_32 lib/tripack/macosx-ix86
