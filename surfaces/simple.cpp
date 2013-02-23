#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"


extern "C" {

  static const char *MyNames[]   = { "aperture" , "R"	, "K" , "n" , "thickness" };
  static const double MyValues[] = { 0.0	, 0.0	, 0.0 , 1.0 , 0.0 };

  int info(int command, char **strings, double **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {
	    int nparams = sizeof(MyNames)/sizeof(char *);

	    *strings = (char *)   MyNames;
	    *values  = (double *) MyValues;

	    return nparams;
        }
    }
  }

  void traverse(double n0, double z, Surface &s, Ray &r)
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
	double denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y);

	if (s.K == -1.0 and denom == 0.0) {	// Special case : Parabola straight in.

	    d = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) - 2*s.R*(r.p(Z) - z))/(2*s.R*r.k(Z));
	} else {
	    denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y) + (s.K + 1) * r.k(Z)*r.k(Z);

	    double b = (r.p(X)*r.k(X) + r.p(Y)*r.k(Y) + ((s.K+1)*(r.p(Z)-z) - s.R)*r.k(Z))/denom;
	    double c = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) +  (s.K+1)*(r.p(Z)*r.p(Z) - 2*r.p(Z)*z + z * z)
			    - 2 * s.R * (r.p(Z)-z))/denom;
	    d = -b - Dsign * Rsign * Ksign * sqrt(b*b - c);
	}
    }

    // Ray/Surface Intersection position
    //
    r.p += d * r.k;



    // Normal
    //
    if ( s.R == 0.0 || s.R > 1.0e10 ) {		// Planar
	nhat = Vector3d(0.0, 0.0, 1.0);
    } else {
	double Rsign = s.R/fabs(s.R);

	nhat = Vector3d(r.p(X), r.p(Y), 
	    -Rsign * sqrt(s.R * s.R - (s.K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y))));
	nhat /= nhat.norm();
    }

    if ( n0 != s.n ) {
	if ( s.n < 0.0 ) {			// Reflect
					    	// http://http.developer.nvidia.com/Cg/reflect.html
	    r.k = r.k - 2 * nhat * (nhat.dot(r.k));
	} else {				// Refract
						// http://http.developer.nvidia.com/Cg/refract.html
	    double eta = n0/s.n;
	    double cosi = (-r.k).dot(nhat);
	    double cost = 1.0 - eta*eta * ( 1.0 - cosi*cosi);

	    r.k = eta*r.k + ((eta*cosi - sqrt(abs(cost))) * nhat) * (cost > 0);
	}
    }
  }
}