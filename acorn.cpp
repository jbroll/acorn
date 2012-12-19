#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"

void prays(Ray *ray, int n)
{
    for ( int i = 0; i < n; i++ ) {
	fprintf(stdout, "%10.5f %10.5f %10.5f\t", ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	fprintf(stdout, "%10.5f %10.5f %10.5f\n", ray[i].k(X), ray[i].k(Y), ray[i].k(Z));
    }
}



int Traverse(double n0, double z, Surface &s, Ray &r)
{
    double d;

    // Intersect
    //
    // establish sign flippy dealies
    //
    double Ksign = 1.0;
    double Dsign = r.k(Z)/fabs(r.k(Z));

    Vector3d nhat;


    if ( s.R == 0.0 )  {			// Planar
	d = (z - r.p(Z))/r.k(Z);
    } else {					// http://www-physics.ucsd.edu/~tmurphy/astr597/exercises/raytrace-3d.pdf
	double Rsign = s.R/fabs(s.R);

	if ( s.K < -1.0 ) { Ksign = -1.0; }

	// solve intersection
	//
	double denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y) + (s.K + 1) * r.k(Z)*r.k(Z);

	double b = (r.p(X)*r.k(X) + r.p(Y)*r.k(Y) + ((s.K+1)*(r.p(Z)-z) - s.R)*r.k(Z))/denom;
	double c = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) +  (s.K+1)*(r.p(Z)*r.p(Z) - 2*r.p(Z)*z + z * z)
			- 2 * s.R * (r.p(Z)-z))/denom;

	d = -b - Dsign * Rsign * Ksign * sqrt(b*b - c);
    }

    // ray/surface intersection position
    //
    r.p += d * r.k;

    // Normal
    //
    if ( s.R == 0.0 ) {				// Planar
	nhat = Vector3d(0.0, 0.0, 1.0);
    } else {
	double Rsign = s.R/fabs(s.R);

	nhat = Vector3d(r.p(X), r.p(Y), 
	    -Rsign * sqrt(s.R * s.R - (s.K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y))));
	nhat /= nhat.norm();
    }


    if ( s.n < 0.0 ) {				// Reflect
						// http://http.developer.nvidia.com/Cg/reflect.html
	r.k = r.k - 2 * nhat * (nhat.dot(r.k));
    } else {					// Refract
						// http://http.developer.nvidia.com/Cg/refract.html
	double eta = n0/s.n;
	double cosi = (-r.k).dot(nhat);
	double cost = 1.0 - eta*eta * ( 1.0 - cosi*cosi);

	r.k = eta*r.k + ((eta*cosi - sqrt(abs(cost))) * nhat) * (cost > 0);
    }
}

main(int argc, char **argv)
{
    Ray r[9];

    int ray = 0;
    for ( int i = -1; i < 2; i++ ) {
	for ( int j = -1; j < 2; j++ ) {
	    r[ray].p = Vector3d(i*25, j*25, 0.0);
	    r[ray].k = Vector3d(0, 0, 1.0);

	    ray++;
	}
    }


    Surface s[2] = {
	// aper,       R,   K,    n,   thick
	//
//	  { 0.0,  -400.0, 0.0, -1.00,  -200.0 }
  	  { 0.0,   400.0, 0.0,  1.25,   200.0 }
	, { 0.0,     0.0, 0.0,  1.00,     0.0 }
    };

    double n = 1.0;		// Air
    double z = 0.0;		//

    for ( int i = 0; i < 2; i++ ) {
	prays(r, 9);
	fprintf(stdout, "\n");
	for ( int j = 0; j < 9; j++ ) {
	    Traverse(n, z, s[i], r[j]);
	}
	n  = s[i].n;
	z += s[i].thickness;
    }

    prays(r, 9);
}

