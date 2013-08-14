
#include <stdio.h>
#include <stdlib.h>

extern void zernik_(int *, double *, double *, double *, double *, double *, int *);

void zernikf_std(double x, double y, int n, double *c, double *z, double *dx, double *dy)
{
	double Z[325];
	double X[325];
	double Y[325];
	int    err = 0;
	int    i;

	//    printf("	%.3f %.3f\n", x, y);

	*z  = 0;
	*dx = 0;
	*dy = 0;

	if ( !n ) { return; };

	zernik_(&n, &x, &y, Z, X, Y, &err);

//	for ( i = 0; i < n; i++ ) {
//	    printf("	% 3d: %.3f %.3f %.3f\n", i+1, Z[i], X[i]*c[i]*2, Y[i]*c[i]*2);
//	}


// 	if ( err != 0 ) { printf("ier %d\n", err); }

	for ( i = 0; i < n; i++ ) {
	    *z  += Z[i]*c[i];
	    *dx += X[i]*c[i] * 2.0;
	    *dy += Y[i]*c[i] * 2.0;
//	    printf("%d %f %f\n",i+1, *dx, *dy);
	}
}

