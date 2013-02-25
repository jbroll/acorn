
BITS=-m32 

all : zernike.o

zerniketable : zerniketable.c
	cc zerniketable.c -o zerniketable

zernike.tab : zerniketable nollorder.sh Makefile
	./zerniketable 100 | ./nollorder.sh > zernike.tab

zernike.c : zernike.tab mkzernike
	./mkzernike > zernike.c 

zernike.o : zernike.c
	cc -c $(BITS) zernike.c -o zernike.o


