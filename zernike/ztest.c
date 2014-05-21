
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void zernike_std(double, double, int, double *, double *, double *, double *);
extern void zernikf_std(double, double, int, double *, double *, double *, double *);

int main() {
	int i;
	int n = 48;
	double c[48];

	for ( i = 0; i < 48; i++ ) { c[i] = drand48()*0.07; }

	double z, dx, dy;

	for ( i = 0; i < 100000; i++ ) {
	   ZERNIKE_STD(drand48()*0.707, drand48()*0.707, n, c, &z, &dx, &dy);
	}

	return 0;
}

