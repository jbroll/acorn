#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"


extern "C" {
#include "../zernike/zernike.h"

    void prays(Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    printf("%5d\t%10.6f\t%10.6f\t%10.6f\t", i, ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	    printf("%10.6f\t%10.6f\t%10.6f\t%d\n",     ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].vignetted);
	}
    }

    enum Px_Local { Pm_R = Px_NParams, Pm_K, Pm_xdecenter, Pm_ydecenter, Pm_nradius, Pm_nzterms, Pm_z1 };

  static const char *MyParamNames[]   = { "R", "K"
          , "xdecenter" , "ydecenter"	, "nradius" , "nzterms"
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
  static const double MyParamValue[] = {  0.0, 0.0
           , 0.0, 0.0, 0.0, 1.0
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


  int info(int command, char **strings, double **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {

	    *strings = (char *)   MyParamNames;
	    *values  = (double *) MyParamValue;

	    return sizeof(MyParamNames)/sizeof(char *);
        }
    }
    return 0;
  }

  void traverse(double n0, double z, Surface &s, Ray &r)
  {
    double d;

    double R = s.p[Pm_R];
    double K = s.p[Pm_K];
    double n = s.p[Px_n];

    // Intersect
    //
    // establish sign flippy dealies
    //
    double Ksign = 1.0;
    double Dsign = r.k(Z)/fabs(r.k(Z));
    double Rsign = R/fabs(R);

    Vector3d nhat;

    // d = AcornSimpleSurfaceDistance(r, z, R, K);

    if ( R == 0.0 )  {			// Planar
	d = (z - r.p(Z))/r.k(Z);
    } else {					// http://www-physics.ucsd.edu/~tmurphy/astr597/exercises/raytrace-3d.pdf

	if ( K < -1.0 ) { Ksign = -1.0; }

	// solve intersection
	//
	double denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y);

	if (K == -1.0 and denom == 0.0) {	// Special case : Parabola straight in.

	    d = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) - 2*R*(r.p(Z) - z))/(2*R*r.k(Z));
	} else {
	    denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y) + (K + 1) * r.k(Z)*r.k(Z);

	    double b = (r.p(X)*r.k(X) + r.p(Y)*r.k(Y) + ((K+1)*(r.p(Z)-z) - R)*r.k(Z))/denom;
	    double c = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) +  (K+1)*(r.p(Z)*r.p(Z) - 2*r.p(Z)*z + z * z)
			    - 2 * R * (r.p(Z)-z))/denom;
	    d = -b - Dsign * Rsign * Ksign * sqrt(b*b - c);
	}
    }

    // Ray/Surface intersection position
    //
    r.p += d * r.k;

    double dx, dy, dz;

    if ( (int) s.p[Pm_nzterms] ) {
	double xdecenter = s.p[Pm_xdecenter];
	double ydecenter = s.p[Pm_ydecenter];
	double   nradius = s.p[Pm_nradius];
	int      nzterms = s.p[Pm_nzterms];

	double tol = 0;

	Ray	Rn = r;

	while ( 1 ) {
	    double dz = zernike_std_value((Rn.p(X) + xdecenter)/nradius, (Rn.p(Y) + ydecenter)/nradius, nzterms, &s.p[Pm_z1]);
			zernike_std_slope((Rn.p(X) + xdecenter)/nradius, (Rn.p(Y) + ydecenter)/nradius, nzterms, &s.p[Pm_z1]
				, &dx, &dy);

	    Rn.p = dz * Rn.p;	// move the ray to the estimate of the deformation surface.

	    if ( abs(dz) < tol ) { break; }
	}
    }

    // Normal
    //
    if ( R == 0.0 || abs(R) > 1.0e10 ) {		// Planar
	nhat = Vector3d(0.0, 0.0, -Dsign*1.0);
    } else {
	double DZ = -Dsign * sqrt(R * R - (K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y)));

	nhat = Vector3d(Rsign*Dsign*r.p(X) + dx*DZ
		      , Rsign*Dsign*r.p(Y) + dy*DZ, DZ);
	nhat /= nhat.norm();
    }

    //prays(&r, 1);

    if      ( n == -1 ) {			// Reflect
					    	// http://http.developer.nvidia.com/Cg/reflect.html
	    r.k = r.k - 2 * nhat * nhat.dot(r.k);
    } else if ( n0 != n ) {			 // Refract
						// http://http.developer.nvidia.com/Cg/refract.html
	//printf("Index %f %f\n", n0, n);

	double eta = n0/n;
	double cosi = (-r.k).dot(nhat);
	double cost = 1.0 - eta*eta * ( 1.0 - cosi*cosi);

	r.k = (eta*r.k + (eta*cosi - sqrt(abs(cost))) * nhat) * (cost > 0);
    }
    //prays(&r, 1);
  }
}
