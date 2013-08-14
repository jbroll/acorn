
OS  =$(shell uname)

FC=gfortran-mp-4.7
#FC=gfortran44
#FC=gfortran-4.6
CFLAGS= -msse -fPIC -O3 $(BITS) 

ZOBJ = lib/$(ARCH)/zernike.o lib/$(ARCH)/zernik_std.o lib/$(ARCH)/zernikx.o lib/$(ARCH)/zernikz.o lib/$(ARCH)/zernik.o lib/$(ARCH)/annulr.o 


zernike.Darwin : 
	@$(MAKE) ARCH=Darwin.i386   BITS=-m32 lib/Darwin.i386/zernike.a
	@$(MAKE) ARCH=Darwin.x86_64 BITS=     lib/Darwin.x86_64/zernike.a

zernike.Linux :
	@$(MAKE) ARCH=Linux.x86_64 BITS=    

zernike.a : lib/$(ARCH)/zernike.a


lib/$(ARCH)/zernike.a : $(ZOBJ)
	ar cr lib/$(ARCH)/zernike.a $(ZOBJ)



zerniketable : zerniketable.c
	cc zerniketable.c -o zerniketable

zernike.tab : zerniketable nollorder.sh Makefile
	./zerniketable 100 | ./nollorder.sh > zernike.tab

zernike.c : zernike.tab mkzernike
	./mkzernike > zernike.c 



ztest_c: ztest.c zernike.o
	cc -DZERNIKE_STD=zernike_std $(CFLAGS) -o ztest_c ztest.c zernike.o -lm

lib/$(ARCH)/zernike.o : zernike.c
	@mkdir -p lib/$(ARCH)
	cc $(CFLAGS) -c zernike.c -o lib/$(ARCH)/zernike.o




ztest_f: ztest.c lib/$(ARCH)/zernik_std.c lib/$(ARCH)/zernik.o
	cc -DZERNIKE_STD=zernikf_std $(CFLAGS) -o ztest_f ztest.c zernik_std.c zernik.o

ztest_x: ztest.c zernik_std.c zernik.o
	cc -DZERNIKE_STD=zernikf_std $(CFLAGS) -o ztest_f ztest.c zernik_std.c zernik.o


lib/$(ARCH)/zernik.o : zernik.f
	@mkdir -p lib/$(ARCH)
	$(FC) $(CFLAGS) -c zernik.f -o lib/$(ARCH)/zernik.o

lib/$(ARCH)/zernikx.o : zernikx.c
	@mkdir -p lib/$(ARCH)
	cc $(CFLAGS) -c zernikx.c -o lib/$(ARCH)/zernikx.o

lib/$(ARCH)/zernikz.o : zernikz.c
	@mkdir -p lib/$(ARCH)
	cc $(CFLAGS) -c zernikz.c -o lib/$(ARCH)/zernikz.o

lib/$(ARCH)/annulr.o : annulr.f
	@mkdir -p lib/$(ARCH)
	$(FC) $(CFLAGS) -c annulr.f -o lib/$(ARCH)/annulr.o

lib/$(ARCH)/zernik_std.o : zernik_std.c
	@mkdir -p lib/$(ARCH)
	cc $(CFLAGS) -c zernik_std.c -o lib/$(ARCH)/zernik_std.o




zcmp : zcmp.c lib/$(ARCH)/zernike.a
	cc $(CFLAGS) -o zcmp zcmp.c lib/$(ARCH)/zernike.a -lm
	
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
