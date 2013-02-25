//  a.V = V (N . S / (N . V))


// http://www.thepolygoners.com/tutorials/lineplane/lineplane.html


#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"

extern "C" {
#include "../zernike/zernike.h"


  static const char *MyNames[]   = {
            "xdecenter" , "ydecenter"	, "nradius" , "nzterms"
	  ,         "z1",  "z2",  "z3",  "z4",  "z5",  "z6",  "z7",  "z8",  "z9"
	  , "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19"
	  , "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29"
	  , "z30", "z31", "z32", "z33", "z34", "z35", "z36", "z37", "z38", "z39"
	  , "z40", "z41", "z42", "z43", "z44", "z45", "z46", "z47", "z48", "z49"
	  , "z50", "z51", "z52", "z53", "z54", "z55", "z56", "z57", "z58", "z59"
	  , "z60", "z61", "z62", "z63", "z64", "z65", "z66", "z67", "z68", "z69"
	  , "z70", "z71", "z72", "z73", "z74", "z75", "z76", "z77", "z78", "z79"
	  , "z80", "z81", "z82", "z83", "z84", "z85", "z86", "z87", "z88", "z89"
	  , "z90", "z91", "z92", "z93", "z94", "z95", "z96", "z97", "z98", "z99"
	  , "z100", "z101", "z102", "z103"
  };
  static const double MyValues[] = {  0.0, 0.0, 0.0, 1.0
           , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0
  };


  int info(int command, const char ***strings, const double **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {
	    int nparams = sizeof(MyNames)/sizeof(char *);

	    *strings = MyNames;
	    *values  = MyValues;

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


    if ( (int) s.p[3] ) {
	double xdecenter = s.p[0];
	double ydecenter = s.p[1];
	double   nradius = s.p[2];
	int      nzterms = s.p[3];

	double dx, dy;
	double dz = zernike_std_value((r.p(X) + xdecenter)/nradius, (r.p(Y) + ydecenter)/nradius, nzterms, &s.p[4]);
		    zernike_std_slope((r.p(X) + xdecenter)/nradius, (r.p(Y) + ydecenter)/nradius, nzterms, &s.p[4], &dx, &dy);

	// Ray/Surface Intersection position at zernike surface
	//
	r.p += -dz * r.k;


	Vector3d zn(dx, dy, dz);		// normal to the zernike surface
	zn /= zn.norm();

	//printf("%f %f %f\n", nhat(X), nhat(Y), nhat(Z));
	nhat += zn;
	nhat /= nhat.norm();
    }

    //printf("%f %f %f\n", zn(X), zn(Y), zn(Z));
    //printf("%f %f %f\n\n", nhat(X), nhat(Y), nhat(Z));



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
}
