
#include <stdio.h>
#include <stdlib.h>

#include "zernike.h"

double drand48();


int main() {
	int i;
	int n = 5;
	double c[48];

	double x;
	double y;

	double c_z = 0.0, c_dx = 0.0, c_dy = 0.0;
	double f_z = 0.0, f_dx = 0.0, f_dy = 0.0;
	double x_z = 0.0, x_dx = 0.0, x_dy = 0.0;
	double z_z = 0.0, z_dx = 0.0, z_dy = 0.0;

        //for ( i = 0; i < n; i++ ) { c[i] = 0.0; }

        for ( i = 0; i < n; i++ ) { c[i] = 1; //drand48()*2;
	}


	for ( i = 0; i < 2; i++ ) {
	    x = drand48()*0.707;
	    y = drand48()*0.707;

	    zernike_std( x,  y, n, c, &c_z, &c_dx, &c_dy);
	    
	    zernikf_std( x,  y, n, c, &f_z, &f_dx, &f_dy);

	    zernikx_std( x,  y, n, c, &x_z, &x_dx, &x_dy);

	    zernikz_std( x,  y, n, c, &z_z, &z_dx, &z_dy);

	    printf("%d	: %5.3f, %5.3f", i, x, y);
	    printf("\n : %10.7f %10.7f %10.7f", c_z, c_dx, c_dy);	
	    printf("\n : %10.7f %10.7f %10.7f", f_z, f_dx, f_dy);	
	    printf("\n : %10.7f %10.7f %10.7f", x_z, x_dx, x_dy);	
	    printf("\n : %10.7f %10.7f %10.7f", z_z, z_dx, z_dy);	
	    printf("\n");


	}

	return 0;
}

