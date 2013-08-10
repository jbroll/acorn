
#include <stdio.h>
#include <stdlib.h>

double drand48();

extern void zernike_std(double, double, int, double *, double *, double *, double *);
extern void zernikf_std(double, double, int, double *, double *, double *, double *);
extern void zernikx_std(double, double, int, double *, double *, double *, double *);

int main() {
	int i;
	int n = 10;
	double c[48];

	double x;
	double y;

	double c_z = 0.0, c_dx = 0.0, c_dy = 0.0;
	double f_z = 0.0, f_dx = 0.0, f_dy = 0.0;
	double x_z = 0.0, x_dx = 0.0, x_dy = 0.0;

        //for ( i = 0; i < n; i++ ) { c[i] = 0.0; }

        for ( i = 0; i < n; i++ ) { c[i] = drand48()*5; }


	for ( i = 0; i < 10000000; i++ ) {
	    x = drand48()*0.707;
	    y = drand48()*0.707;

	    zernike_std( x,  y, n, c, &c_z, &c_dx, &c_dy);
	    
	    zernikf_std( x,  y, n, c, &f_z, &f_dx, &f_dy);

	    zernikx_std( x,  y, n, c, &x_z, &x_dx, &x_dy);

	    printf("%d	: %5.3f, %5.3f", i, x, y);
	    printf(" : %10.7f %10.7f %10.7f", c_z, c_dx, c_dy);	
	    printf(" : %10.7f %10.7f %10.7f", f_z, f_dx, f_dy);	
	    printf(" : %10.7f %10.7f %10.7f", x_z, x_dx, x_dy);	
	    printf("\n");


	}

	return 0;
}

