


OS  =$(shell uname)


INC= -I/Users/john/include -I/home/john/include

CFLAGS=$(INC) -msse -fPIC -O2 

SRC	= acorn.cpp aperture.cpp glass/glass.c glass/acorn-glass.cpp tpool/tpool.c
TCL	= acorn.tcl acorn-model.tcl			\
	  zmx-io/uda.tcl zmx-io/agf.tcl zmx-io/zmx.tcl	\
	  util/unix.tcl  util/func.tcl  util/tcloo.tcl	\
	  tcltest.tcl


SURFS	= surfaces/lib/$(ARCH)/simple.so		\
	  surfaces/lib/$(ARCH)/zernike.so		\
	  surfaces/lib/$(ARCH)/lens-array-rect.so	\
	  surfaces/lib/$(ARCH)/lens-array-hex.so	\
	  surfaces/lib/$(ARCH)/evenasph.so		\
	  surfaces/lib/$(ARCH)/dgrating.so

ACORN_SRCS = $(SRC) $(TCL)
ACORN_OBJS = $(SURFS) 
ACORN_UTIL = surfaces/acorn-utils.h


all: zernike acorn.$(OS)

acorn.Darwin :
	@$(MAKE) ARCH=Darwin.i386	BITS=-m32	acorn.Darwin.i386 
	@$(MAKE) ARCH=Darwin.x86_64	BITS=-m64	acorn.Darwin.x86_64 
	@$(MAKE) ARCH=Darwin.i386	BITS=-m32	arec.Darwin.i386 
	@$(MAKE) ARCH=Darwin.x86_64	BITS=-m64	arec.Darwin.x86_64 
	#@$(MAKE) test.Darwin

acorn.Linux  :
	@$(MAKE) ARCH=Linux.x86_64	acorn.Linux.x86_64
	@$(MAKE) ARCH=Linux.x86_64	arec.Linux.x86_64
	#@$(MAKE) ARCH=Linux.x86_64	test.Linux

zernike/zernike.c :
	cd zernike; $(MAKE)

acorn.Darwin.i386   : lib/acorn/macosx-ix86/acorn.dylib		arec.Darwin.i386	$(ACORN_OBJS)
acorn.Darwin.x86_64 : lib/acorn/macosx-x86_64/acorn.dylib	arec.Darwin.x86_64	$(ACORN_OBJS)
acorn.Linux.x86_64  : lib/acorn/linux-x86_64/acorn.so		arec.Linux.x86_64	$(ACORN_OBJS)

lib/acorn/macosx-ix86/acorn.dylib   :	$(ACORN_SRCS)
	ARCH=Darwin.i386 critcl -target macosx-x86_32 -pkg acorn 
	rm -rf lib/acorn/macosx-ix86
	mv lib/acorn/macosx-x86_32 lib/acorn/macosx-ix86

lib/acorn/macosx-x86_64/acorn.dylib :	$(ACORN_SRCS)
	ARCH=Darwin.x86_64 critcl -target macosx-x86_64 -pkg acorn 

lib/acorn/linux-x86_64/acorn.so :	$(ACORN_SRCS)
	ARCH=$(ARCH) critcl -pkg acorn 



surfaces/lib/$(ARCH)/simple.so : surfaces/simple.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/simple.cpp -o surfaces/lib/$(ARCH)/simple.so

surfaces/lib/$(ARCH)/evenasph.so : surfaces/evenasph.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/evenasph.cpp -o surfaces/lib/$(ARCH)/evenasph.so

surfaces/lib/$(ARCH)/dgrating.so : surfaces/dgrating.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/dgrating.cpp -o surfaces/lib/$(ARCH)/dgrating.so

surfaces/lib/$(ARCH)/zernike.so : surfaces/zernike.cpp acorn.h zernike/lib/$(ARCH)/zernike.a $(ACORN_UTIL)
	@mkdir -p zernike/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/zernike.cpp zernike/lib/$(ARCH)/zernike.a -o surfaces/lib/$(ARCH)/zernike.so
	
surfaces/lib/$(ARCH)/lens-array-rect.so : surfaces/lens-array-rect.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/lens-array-rect.cpp -o surfaces/lib/$(ARCH)/lens-array-rect.so
	
surfaces/lib/$(ARCH)/lens-array-hex.so : surfaces/lens-array-hex.cpp acorn.h $(ACORN_UTIL)
	@mkdir -p surfaces/lib/$(ARCH)
	g++ $(BITS) -fPIC -O2 -shared $(INC) surfaces/lens-array-hex.cpp -o surfaces/lib/$(ARCH)/lens-array-hex.so

tpool/lib/$(ARCH)/tpool.o : tpool/tpool.c
	@mkdir -p tpool/lib/$(ARCH)
	gcc $(BITS) -fPIC -O2 -c $(INC) tpool/tpool.c -o tpool/lib/$(ARCH)/tpool.o

glass/lib/$(ARCH)/glass.o : glass/glass.c
	@mkdir -p glass/lib/$(ARCH)
	gcc $(BITS) -fPIC -O2 -c $(INC) glass/glass.c -o glass/lib/$(ARCH)/glass.o

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
	arch -i386   /usr/local/bin/tclsh8.6 ./acorn-test.tcl 
	arch -x86_64 /usr/local/bin/tclsh8.6 ./acorn-test.tcl

test.Linux : FORCE
	tclsh8.6 ./acorn-test.tcl

clean : 
	$(MAKE) ARCH=Darwin.i386	clean-rm
	$(MAKE) ARCH=Darwin.x86_64	clean-rm
	$(MAKE) ARCH=Linux.i386		clean-rm
	$(MAKE) ARCH=Linux.x86_64	clean-rm


clean.Darwin.i386: ARCH = Darwin.i368
clean-rm :
	rm -f *.o $(SURFS)


FORCE:
