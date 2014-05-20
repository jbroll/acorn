
#include <stdio.h>
#include <stdlib.h>

#include "zernike.h"

double drand48();

#define NTERMS 104

int main() {
	int i, z;
	int n = NTERMS;
	double c[NTERMS];

	double x;
	double y;

	double c_z = 0.0, c_dx = 0.0, c_dy = 0.0;
	double f_z = 0.0, f_dx = 0.0, f_dy = 0.0;
	double x_z = 0.0, x_dx = 0.0, x_dy = 0.0;
	double z_z = 0.0, z_dx = 0.0, z_dy = 0.0;

        //for ( i = 0; i < n; i++ ) { c[i] = 0.0; }



    for ( z = 0; z < n; z++ ) {

        for ( i = 0; i < n; i++ ) {
	    if ( i == z ) {
		c[i] = 0.25;
	    } else {
		c[i] = 0.0;
	    }
	}

	
	for ( i = 0; i < 100; i++ ) {
	    x = drand48()*0.707;
	    y = drand48()*0.707;

	    zernike_std( x,  y, n, c, &c_z, &c_dx, &c_dy);

	    zernikf_std( x,  y, n, c, &f_z, &f_dx, &f_dy);
	    zernikx_std( x,  y, n, c, &x_z, &x_dx, &x_dy);
	    zernikz_std( x,  y, n, c, &z_z, &z_dx, &z_dy);

	    printf("%d	: %5.3f, %5.3f", z+1, x, y);
	    printf(" : %10.5f %10.5f %10.5f", f_z, f_dx, f_dy);	
	    printf(" : %10.5f %10.5f %10.5f", x_z-f_z, x_dx-f_dx, x_dy-f_dy);	
	    printf(" : %10.5f %10.5f %10.5f", z_z-f_z, z_dx-f_dx, z_dy-f_dy);	
	    printf(" : %10.5f %10.5f %10.5f", c_z-f_z, c_dx-f_dx, c_dy-f_dy);	
	    printf("\n");
	}
    }

    exit(0);
}

