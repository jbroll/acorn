
FC=gfortran-mp-4.7 -m32 -g -O3
CFLAGS= -m32 -O3

BITS=-m32 

all : zernike.c

zerniketable : zerniketable.c
	cc zerniketable.c -o zerniketable

zernike.tab : zerniketable nollorder.sh Makefile
	./zerniketable 100 | ./nollorder.sh > zernike.tab

zernike.c : zernike.tab mkzernike
	./mkzernike > zernike.c 

