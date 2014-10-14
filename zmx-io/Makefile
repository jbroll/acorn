
CXX=/usr/local/bin/g++
CXX=/usr/bin/g++

zmx.o : zmx.cc
	$(CXX) -std=c++0x $<

zmx.C : FORCE
	grep Keyword zmx.C | grep -v define | awk '{ print comma, "{ \""$$2"\", &ZMX::"$$2" }"; comma="," }' > zmx.mtable

FORCE:
