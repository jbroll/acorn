
zmx.o : zmx.C
	c++ -std=c++0x $<

zmx.C : FORCE
	grep Keyword zmx.C | grep -v define | awk '{ print comma, "{ \""$$2"\", &ZMX::"$$2" }"; comma="," }' > zmx.mtable
	grep Param   zmx.C | grep -v define | awk '{ gsub(";", ""); print comma, "{  \""$$3"\", { Type_"$$2", offsetof(ZMX, "$$3") } }" ; comma="," }' > zmx.vtable

FORCE:
