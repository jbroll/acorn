#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"
#include "acorn-utils.h" 

extern "C" {
#include "../zernike/zernike.h"

    void drays(const char *msg, Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    fprintf(stderr, "%s %5d\t%10.6f\t%10.6f\t%10.6f\t%10.6f\t%10.6f\t%10.6f\t%d\n"
		    , msg, i, ray[i].p(X), ray[i].p(Y), ray[i].p(Z)
		       , ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].vignetted);
	    fflush(stderr);
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

  int traverse(double n0, double z, Surface &s, Ray &r)
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

    double xdecenter = s.p[Pm_xdecenter];
    double ydecenter = s.p[Pm_ydecenter];
    double   nradius = s.p[Pm_nradius];
    int      nzterms = s.p[Pm_nzterms];

    double tol	     = 0.0000000000001;

    d = AcornSimpleSurfaceDistance(r, z, R, K);

    // Ray/Surface intersection position
    //
    r.p += d * r.k;


    if ( nzterms ) {
	    double zstart = r.p(Z), zz, dx, dy, dz;

	for ( int iter = 0; iter < 5; iter++ ) {
	    zernike_std((r.p(X) + xdecenter)/nradius, (r.p(Y) + ydecenter)/nradius, nzterms, &s.p[Pm_z1], &zz, &dx, &dy);

	    Vector3d P = Vector3d(r.p(X), r.p(Y), zstart+zz);		// Estimate point on the surface.

	    								// Compute the normal to the conic + zernike
	    if ( R == 0.0 || abs(R) > 1.0e10 ) {			// Planar
		nhat = Vector3d(Rsign*Dsign*dx, Rsign*Dsign*dy, -Dsign*1.0);
	    } else {
		double dz = sqrt(R * R - (K+1)*(P(X) * P(X) + P(Y) * P(Y)));

		//nhat = Vector3d(Rsign*Dsign*P(X) + dx*zz, Rsign*Dsign*P(Y) + dy*zz, -Dsign * dz);
		nhat = Vector3d(Rsign*Dsign*P(X), Rsign*Dsign*P(Y), -Dsign * dz);
	    }
	    nhat /= nhat.norm();


	    double Num = (P-r.p).dot(nhat);
	    double Den =     r.k.dot(nhat);

	    if ( Den != 0.0f ) {
		r.p += Num/Den * r.k;					// Move along the ray the distance to the normal surface.

		//printf("%d zz %.10f d %.10f	", iter, zz, Num/Den);
		//printf("%f %f %f\n", r.p(X), r.p(Y), r.p(Z));

		if ( abs(Num/Den) < tol ) {
		    //printf("Break %f %f\n", Num/Den , tol);

		    break;
		}				// If the ray failes to advance, then Done.
	    } else {
		return 1;						// BANG!
	    }
	}
    } else {
	nhat = AcornSimpleSurfaceNormal(r, R, K);
    }

    if ( aper_clip(&s, &r) ) { return 1; }

    //printf("%f %f %f\n", nhat(X), nhat(Y), nhat(Z));
    //drays("1", &r, 1);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract
    //drays("2", &r, 1);

    return 0;
  }
}
