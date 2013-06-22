
FC=gfortran-mp-4.7
#FC=gfortran-4.6
CFLAGS= -m32 -O3 

all : zernike.c

zerniketable : zerniketable.c
	cc zerniketable.c -o zerniketable

zernike.tab : zerniketable nollorder.sh Makefile
	./zerniketable 100 | ./nollorder.sh > zernike.tab

zernike.c : zernike.tab mkzernike
	./mkzernike > zernike.c 



ztest_c: ztest.c zernike.o
	cc -DZERNIKE_STD=zernike_std $(CFLAGS) -o ztest_c ztest.c zernike.o -lm

zernike.o : zernike.c
	cc $(CFLAGS) -c zernike.c 




ztest_f: ztest.c zernik_std.c zernik.o
	cc -DZERNIKE_STD=zernikf_std $(CFLAGS) -o ztest_f ztest.c zernik_std.c zernik.o

zernik.o : zernik.f
	$(FC) $(CFLAGS) -c zernik.f

zcmp : zcmp.c zernik_std.c zernik.o zernike.o
	cc $(CFLAGS) -o zcmp zcmp.c zernike.o zernik_std.c zernik.o -lm
	
test: ztest_c ztest_f
	rm -f gmon.out ; time ./ztest_c #; gprof ztest_c > ztest_c.gmon
	rm -f gmon.out ; time ./ztest_f #; gprof ztest_f > ztest_f.gmon

