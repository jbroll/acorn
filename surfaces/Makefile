
CXX=/usr/local/bin/g++
CXX=/usr/bin/g++

CXXFLAGS= -std=c++0x

OS  =$(shell uname)


INC= -I/Users/john/include -I/home/john/include -I/home/jroll/include
JLIBS= -L/Users/john/lib   -L/data/mmti/lib     -L/home/jroll/lib

CFLAGS=$(INC) -msse -fPIC -O3 $(BITS) 

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

simple.vtable : simple.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceSimple, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceSimple, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/simple.so : simple.cc simple.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


coordbrk.vtable : coordbrk.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceCoordBrk, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceCoordBrk, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/coordbrk.so : coordbrk.cc coordbrk.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


tilted.vtable : tilted.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceTilted, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceTilted, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/tilted.so : tilted.cc tilted.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


zernike.vtable : zernike.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceZernike, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceZernike, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/zernike.so : zernike.cc zernike.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@



zernikz.vtable : zernikz.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceZernike, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceZernike, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/zernikz.so : zernikz.cc zernikz.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


lens-array-rect.vtable : lens-array-rect.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceLensArrayRect, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceLensArrayRect, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/lens-array-rect.so : lens-array-rect.cc lens-array-rect.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


lens-array-hex.vtable : lens-array-hex.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceLensArrayHex, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceLensArrayHex, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/lens-array-hex.so : lens-array-hex.cc lens-array-hex.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


evenasph.vtable : evenasph.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceEvenASphere, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceEvenASphere, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/evenasph.so : evenasph.cc evenasph.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@


dgrating.vtable : dgrating.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceDGrating, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceDGrating, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/dgrating.so : dgrating.cc dgrating.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@

QE.vtable : QE.cc ../AcornSurface.hh
	grep Param   ../AcornSurface.hh | grep -v define | awk -v comma=" " '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceQE, "$$3") } }" ; comma="," }' >  $@
	grep Param                   $< | grep -v define | awk -v comma=,   '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(AcornSurfaceQE, "$$3") } }" ; comma="," }' >> $@
	

lib/$(ARCH)/QE.so : QE.cc QE.vtable ../Acorn.hh $(ACORN_UTIL) 
	@mkdir -p lib/$(ARCH)
	$(CXX) -c $(CXXFLAGS) -shared $(INC) $< -o $@

