# This is the main rules makefile for all of starbase
#

CFLAGS	= -D$(OS) $(PK_DEFS)   $(OS_DEFS)   $(OS_CFLAGS) $(PK_CFLAGS) $(OS_INCL) $(PK_INCL)
LDLIBS	=         $(PK_LDLIBS) $(OS_LDLIBS)

dummy:

distclean::
	rm -f Makefile

clean::
	rm -f *.a *.o *.bak

.c.o:
	$(CC) -c $(CPPFLAGS) $(INCL) $(CFLAGS) $<

.l.o:
	flex  -t $< > $*.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $*.c


