
#include <stdio.h>
#include <stdlib.h>

double drand48();

extern void zernike_std(double, double, int, double *, double *, double *, double *);
extern void zernikf_std(double, double, int, double *, double *, double *, double *);

int main() {
	int i;
	int n = 10;
	double c[48];

	double x;
	double y;

	double c_z = 0.0, c_dx = 0.0, c_dy = 0.0;
	double f_z = 0.0, f_dx = 0.0, f_dy = 0.0;

        //for ( i = 0; i < n; i++ ) { c[i] = 0.0; }

        for ( i = 0; i < n; i++ ) { c[i] = drand48()*5; }


	for ( i = 0; i < 10000000; i++ ) {
	    x = drand48()*0.707;
	    y = drand48()*0.707;

	    zernike_std( x,  y, n, c, &c_z, &c_dx, &c_dy);
	    
	    zernikf_std( x,  y, n, c, &f_z, &f_dx, &f_dy);

	    //printf("%d	: %.3f, %.3f : %.7f %.7f %.7f : %.7f %.7f %.7f\n", i, x, y, c_z, c_dx, c_dy, f_z, f_dx, f_dy);	


	}

	return 0;
}

