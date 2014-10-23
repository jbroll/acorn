
CXX=/usr/local/bin/g++
CXX=/usr/bin/g++

INC= -I/Users/john/include -I/home/john/include -I/home/jroll/include

all : uda.o agf.o zmx.o

agf.o : agf.cc
	$(CXX) $(INC) -std=c++0x $<

agf.cc : FORCE
	grep Keyword agf.cc | grep -v define | awk '{ print comma, "{ \""$$2"\", &AGF::"$$2" }"; comma="," }' > agf.mtable

uda.o : uda.cc
	$(CXX) $(INC) -std=c++0x $<

uda.cc : FORCE
	grep Keyword uda.cc | grep -v define | awk '{ print comma, "{ \""$$2"\", &UDA::"$$2" }"; comma="," }' > uda.mtable

zmx.o : zmx.cc
	$(CXX) $(INC) -std=c++0x $<

zmx.cc : FORCE
	grep Keyword zmx.cc | grep -v define | awk '{ print comma, "{ \""$$2"\", &ZMX::"$$2" }"; comma="," }' > zmx.mtable

FORCE:
