
OS  =$(shell uname)

CFLAGS= -fPIC -O3 $(BITS) 


ZOBJ = lib/$(ARCH)/zernike.o lib/$(ARCH)/zernik_std.o lib/$(ARCH)/zernikx.o lib/$(ARCH)/zernikz.o  
#ZOBJ = lib/$(ARCH)/zernike.o lib/$(ARCH)/zernik_std.o lib/$(ARCH)/zernikx.o lib/$(ARCH)/zernikz.o lib/$(ARCH)/zernik.o lib/$(ARCH)/annulr.o 

all: zernike.$(OS)

zernike.Darwin : 
	@$(MAKE) ARCH=Darwin.i386   BITS=-m32 lib/Darwin.i386/zernike.a
	@$(MAKE) ARCH=Darwin.x86_64 BITS=     lib/Darwin.x86_64/zernike.a

zernike.Linux :
	@$(MAKE) ARCH=Linux.x86_64 BITS=    zernike.a

zernike.a : $(ZOBJ) lib/$(ARCH)/zernike.a


lib/$(ARCH)/zernike.a : $(ZOBJ)
	ar cr lib/$(ARCH)/zernike.a $(ZOBJ)


zerniketable : zerniketable.c
	$(CC) zerniketable.c -o zerniketable

zernike.tab : zerniketable nollorder.sh Makefile
	./zerniketable 100 | ./nollorder.sh > zernike.tab

zernike.c : zernike.tab mkzernike
	./mkzernike > zernike.c 

lib/$(ARCH)/zernike.o : zernike.c
	@mkdir -p lib/$(ARCH)
	$(CC) $(CFLAGS) -c zernike.c -o lib/$(ARCH)/zernike.o



ztest: ztest_c ztest_z
	echo OK

ztest_c: ztest.c 
	$(CC) -DZERNIKE_STD=zernike_std $(CFLAGS) -o ztest_c ztest.c lib/$(ARCH)/zernike.a -lm

ztest_z: ztest.c 
	$(CC) -DZERNIKE_STD=zernikz_std $(CFLAGS) -o ztest_z ztest.c lib/$(ARCH)/zernike.a -lm


lib/$(ARCH)/zernik.o : zernik.f
	@mkdir -p lib/$(ARCH)
	$(FC) $(CFLAGS) -c zernik.f -o lib/$(ARCH)/zernik.o

lib/$(ARCH)/zernikx.o : zernikx.c
	@mkdir -p lib/$(ARCH)
	$(CC) $(CFLAGS) -c zernikx.c -o lib/$(ARCH)/zernikx.o

lib/$(ARCH)/zernikz.o : zernikz.c
	@mkdir -p lib/$(ARCH)
	$(CC) $(CFLAGS) -c zernikz.c -o lib/$(ARCH)/zernikz.o

lib/$(ARCH)/annulr.o : annulr.f
	@mkdir -p lib/$(ARCH)
	$(FC) $(CFLAGS) -c annulr.f -o lib/$(ARCH)/annulr.o

lib/$(ARCH)/zernik_std.o : zernik_std.c
	@mkdir -p lib/$(ARCH)
	$(CC) $(CFLAGS) -c zernik_std.c -o lib/$(ARCH)/zernik_std.o




zcmp : zcmp.c lib/$(ARCH)/zernike.a
	$(CC) $(CFLAGS) -o zcmp zcmp.c lib/$(ARCH)/zernike.a -lm
	
test: ztest_c ztest_f
	rm -f gmon.out ; time ./ztest_c #; gprof ztest_c > ztest_c.gmon
	rm -f gmon.out ; time ./ztest_f #; gprof ztest_f > ztest_f.gmon


clean : 
	$(MAKE) ARCH=Darwin.i386	clean-rm
	$(MAKE) ARCH=Darwin.x86_64	clean-rm
	$(MAKE) ARCH=Linux.i386		clean-rm
	$(MAKE) ARCH=Linux.x86_64	clean-rm

clean-rm :
	rm -f lib/$(ARCH)/*.o lib/$(ARCH)/*.a
