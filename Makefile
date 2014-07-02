

OS  =$(shell uname)


INC= -I/Users/john/include -I/home/john/include

CFLAGS=$(INC) -msse -fPIC -O3 $(BITS) 

SRC	= acorn.cpp acorn.h aperture.cpp glass/glass.c glass/acorn-glass.cpp tpool/tpool.c
TCL	= acorn.tcl acorn-model.tcl				\
	  zmx-io/uda.tcl zmx-io/agf.tcl zmx-io/zmx.tcl		\
	  jbr.tcl/unix.tcl  jbr.tcl/func.tcl  jbr.tcl/tcloo.tcl	\
	  util/rays.tcl						\
	  tcltest.tcl


SURFS	= surfaces/lib/$(ARCH)/simple.so		\
	  surfaces/lib/$(ARCH)/coordbrk.so		\
	  surfaces/lib/$(ARCH)/tilted.so		\
	  surfaces/lib/$(ARCH)/zernike.so		\
	  surfaces/lib/$(ARCH)/zernikz.so		\
	  surfaces/lib/$(ARCH)/lens-array-rect.so	\
	  surfaces/lib/$(ARCH)/lens-array-hex.so	\
	  surfaces/lib/$(ARCH)/evenasph.so		\
	  surfaces/lib/$(ARCH)/dgrating.so

ACORN_SRCS = $(SRC) $(TCL)
ACORN_OBJS = $(SURFS) 
ACORN_UTIL = surfaces/acorn-utils.h


all: acorn.$(OS)

acorn.Darwin :
	#@$(MAKE) ARCH=Darwin.i386	BITS=-m32	acorn.Darwin.i386   rays.Darwin.i386   nproc.Darwin.i386   arec.Darwin.i386
	@$(MAKE) ARCH=Darwin.x86_64	BITS=-m64	acorn.Darwin.x86_64 rays.Darwin.x86_64 nproc.Darwin.x86_64 arec.Darwin.x86_64

acorn.Linux  :
	@$(MAKE) ARCH=Linux.x86_64	acorn.Linux.x86_64 rays.Linux.x86_64 nproc.Linux.x86_64 arec.Linux.x86_64 
	#@$(MAKE) ARCH=Linux.x86_64	test.Linux


acorn.Darwin.i386   : lib/acorn/macosx-ix86/acorn.dylib		$(ACORN_OBJS)
acorn.Darwin.x86_64 : lib/acorn/macosx-x86_64/acorn.dylib	$(ACORN_OBJS)
acorn.Linux.x86_64  : lib/acorn/linux-x86_64/acorn.so		$(ACORN_OBJS)

rays.Darwin.i386   : lib/rays/macosx-ix86/rays.dylib
rays.Darwin.x86_64 : lib/rays/macosx-x86_64/rays.dylib
rays.Linux.x86_64  : lib/rays/linux-x86_64/rays.so

lib/acorn/macosx-ix86/acorn.dylib   :	$(ACORN_SRCS)
	ARCH=Darwin.i386 critcl -target macosx-x86_32 -pkg acorn 

lib/acorn/macosx-x86_64/acorn.dylib :	$(ACORN_SRCS)
	ARCH=Darwin.x86_64 critcl -target macosx-x86_64 -pkg acorn 

lib/acorn/linux-x86_64/acorn.so :	$(ACORN_SRCS)
	ARCH=$(ARCH) ./critcl31.kit -pkg acorn 


lib/rays/macosx-ix86/rays.dylib   :	rays.tcl rays.h
	ARCH=Darwin.i386 critcl -target macosx-x86_32 -pkg rays 

lib/rays/macosx-x86_64/rays.dylib :	rays.tcl rays.h
	ARCH=Darwin.x86_64 critcl -target macosx-x86_64 -pkg rays 

lib/rays/linux-x86_64/rays.so :	rays.tcl rays.h
	ARCH=linux.x86_64 critcl -pkg rays 



nproc.Darwin.i386   : lib/nproc/macosx-ix86/nproc.dylib	
nproc.Darwin.x86_64 : lib/nproc/macosx-x86_64/nproc.dylib
nproc.Linux.x86_64  : lib/nproc/linux-x86_64/nproc.so

lib/nproc/macosx-ix86/nproc.dylib   :	nproc.tcl
	ARCH=Darwin.i386 critcl -target macosx-x86_32 -pkg nproc 

lib/nproc/macosx-x86_64/nproc.dylib :	nproc.tcl
	ARCH=Darwin.x86_64 critcl -target macosx-x86_64 -pkg nproc 

lib/nproc/linux-x86_64/nproc.so :	nproc.tcl
	ARCH=$(ARCH) critcl -pkg nproc 



surfaces/lib/$(ARCH)/simple.so : surfaces/simple.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/simple.cpp -o surfaces/lib/$(ARCH)/simple.so

surfaces/lib/$(ARCH)/coordbrk.so : surfaces/coordbrk.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/coordbrk.cpp -o surfaces/lib/$(ARCH)/coordbrk.so

surfaces/lib/$(ARCH)/tilted.so : surfaces/tilted.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/tilted.cpp -o surfaces/lib/$(ARCH)/tilted.so


surfaces/lib/$(ARCH)/evenasph.so : surfaces/evenasph.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/evenasph.cpp -o surfaces/lib/$(ARCH)/evenasph.so

surfaces/lib/$(ARCH)/dgrating.so : surfaces/dgrating.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/dgrating.cpp -o surfaces/lib/$(ARCH)/dgrating.so

surfaces/lib/$(ARCH)/zernike.so : surfaces/zernike.cpp acorn.h zernike/lib/$(ARCH)/zernike.a $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/zernike.cpp zernike/lib/$(ARCH)/zernike.a -o surfaces/lib/$(ARCH)/zernike.so
	
surfaces/lib/$(ARCH)/zernikz.so : surfaces/zernikz.cpp acorn.h zernike/lib/$(ARCH)/zernike.a $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/zernikz.cpp zernike/lib/$(ARCH)/zernike.a -o surfaces/lib/$(ARCH)/zernikz.so
	
surfaces/lib/$(ARCH)/lens-array-rect.so : surfaces/lens-array-rect.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/lens-array-rect.cpp -o surfaces/lib/$(ARCH)/lens-array-rect.so
	
surfaces/lib/$(ARCH)/lens-array-hex.so : surfaces/lens-array-hex.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	$(CXX) $(CFLAGS) -shared $(INC) surfaces/lens-array-hex.cpp -o surfaces/lib/$(ARCH)/lens-array-hex.so

tpool/lib/$(ARCH)/tpool.o : tpool/tpool.c
	@mkdir -p tpool/lib/$(ARCH)
	$(CC) $(CFLAGS) -c $(INC) tpool/tpool.c -o tpool/lib/$(ARCH)/tpool.o

glass/lib/$(ARCH)/glass.o : glass/glass.c
	@mkdir -p glass/lib/$(ARCH)
	$(CC) $(CFLAGS) -c $(INC) glass/glass.c -o glass/lib/$(ARCH)/glass.o

zernike/lib/$(ARCH)/zernike.a : FORCE
	cd zernike; $(MAKE) ARCH=$(ARCH) lib/$(ARCH)/zernike.a

arec.Darwin.i386	:
	@cd arec; $(MAKE) arec.Darwin.i386

arec.Darwin.x86_64	:
	@cd arec; $(MAKE) arec.Darwin.x86_64

arec.Linux.x86_64 	:
	@cd arec; $(MAKE) arec.Linux.x86_64

test : test.$(OS)

test.Darwin : FORCE
	#arch -i386  /usr/local/bin/tclsh8.6 ./acorn-test.tcl 
	arch -x86_64 /usr/local/bin/tclsh8.6 ./acorn-test.tcl

test.Linux : FORCE
	tclkit8.6.1 ./acorn-test.tcl

clean : 
	$(MAKE) ARCH=Darwin.i386	clean-rm
	$(MAKE) ARCH=Darwin.x86_64	clean-rm
	$(MAKE) ARCH=Linux.i386		clean-rm
	$(MAKE) ARCH=Linux.x86_64	clean-rm
	cd zernike ; $(MAKE) clean


clean.Darwin.i386: ARCH = Darwin.i368
clean-rm :
	rm -f *.o $(SURFS)


FORCE:
