
CXX=/usr/local/bin/g++
CXX=/usr/bin/g++


OS  =$(shell uname)


INC= -I/Users/john/include -I/home/john/include -I/home/jroll/include
JLIBS= -L/Users/john/lib   -L/data/mmti/lib     -L/home/jroll/lib

CFLAGS=$(INC) -msse -fPIC -O3 $(BITS) 
CXXFLAGS=$(INC) -std=c++0x -msse -fPIC -O3

SRC	= acorn.cpp acorn.h aperture.cpp glass/glass.c glass/acorn-glass.cpp tpool/tpool.c


SURFS	= lib/$(ARCH)/simple.so			\
	  lib/$(ARCH)/coordbrk.so		\
	  lib/$(ARCH)/tilted.so			\
	  lib/$(ARCH)/zernike.so		\
	  lib/$(ARCH)/zernikz.so		\
	  lib/$(ARCH)/lens-array-rect.so	\
	  lib/$(ARCH)/lens-array-hex.so		\
	  lib/$(ARCH)/evenasph.so		\
	  lib/$(ARCH)/dgrating.so		\
	  lib/$(ARCH)/QE.so


ACORN_SRCS = $(SRC) $(TCL)
ACORN_OBJS = $(SURFS) 
ACORN_UTIL = acorn-utils.h

all: $(SURFS)


%.vtable : %.cc
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(MyClass, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(MyClass, "$$3") } }" ; comma="," }' >> $@
	
lib/$(ARCH)/%.so : %.cc
	@mkdir -p lib/$(ARCH)
	$(CXX) $(CXXFLAGS) -shared $(INC) $< -o $@


lib/$(ARCH)/simple.so   : simple.cc   simple.vtable   ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/coordbrk.so : coordbrk.cc coordbrk.vtable ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/tilted.so   : tilted.cc   tilted.vtable   ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/zernike.so  : zernike.cc  zernike.vtable  ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/zernikz.so  : zernikz.cc  zernikz.vtable  ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/evenasph.so : evenasph.cc evenasph.vtable ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/dgrating.so : dgrating.cc dgrating.vtable ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/QE.so 	: QE.cc       QE.vtable       ../Acorn.hh $(ACORN_UTIL) 

lib/$(ARCH)/lens-array-rect.so : lens-array-rect.cc lens-array-rect.vtable ../Acorn.hh $(ACORN_UTIL) 
lib/$(ARCH)/lens-array-hex.so  : lens-array-hex.cc  lens-array-hex.vtable  ../Acorn.hh $(ACORN_UTIL) 

