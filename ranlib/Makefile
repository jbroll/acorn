

libranlib.a : linpack/linpack.c src/ranlib.c src/com.c
	cc -O3 -fPIC -c linpack/linpack.c src/ranlib.c src/com.c 
	ar qc libranlib.a linpack.o ranlib.o com.o

clean:
	rm *.a
