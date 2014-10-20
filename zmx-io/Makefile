
CXX=/usr/local/bin/g++
CXX=/usr/bin/g++

INC= -I/Users/john/include -I/home/john/include -I/home/jroll/include

agf.o : zmx.cc
	$(CXX) $(INC) -std=c++0x $<

zmx.C : FORCE
	grep Keyword zmx.C | grep -v define | awk '{ print comma, "{ \""$$2"\", &ZMX::"$$2" }"; comma="," }' > zmx.mtable


zmx.o : zmx.cc
	$(CXX) $(INC) -std=c++0x $<

zmx.C : FORCE
	grep Keyword zmx.C | grep -v define | awk '{ print comma, "{ \""$$2"\", &ZMX::"$$2" }"; comma="," }' > zmx.mtable

FORCE:
