
arec	= lib/arec/pkgIndex.tcl
acorn	= lib/acorn/pkgIndex.tcl

$(acorn) : acorn.h acorn.tcl
	critcl31 -target macosx-x86_32 -force -pkg acorn
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86

$(arec) : arec.c arec.h arec.tcl
	critcl31 -target macosx-x86_32 -force -pkg arec
	rm -rf lib/arec/macosx-ix86
	mv lib/arec/macosx-x86_32 lib/arec/macosx-ix86

acorn.o : acorn.cpp
	g++ -c -I /Users/john/include acorn.cpp -o acorn.o

