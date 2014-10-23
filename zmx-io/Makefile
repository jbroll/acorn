

CXXFLAGS=$(INC) -std=c++0x

INC= -I/Users/john/include -I/home/john/include -I/home/jroll/include

all : uda.o agf.o zmx.o

agf.o :  agf.mtable
uda.o :  uda.mtable
zmx.o :  zmx.mtable


%.mtable : %.cc
	grep Keyword $< | grep -v define | awk '{ print comma, "{ \""$$2"\", &MyClass::"$$2" }"; comma="," }' > $@


