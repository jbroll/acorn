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

    d = AcornSimpleSurfaceDistance(r, z, R, K);

    // Ray/Surface intersection position
    //
    r.p += d * r.k;

    if ( aper_clip(&s, &r) ) { return 1; }

    double dx, dy, dz;

    if ( (int) s.p[Pm_nzterms] ) {
	double xdecenter = s.p[Pm_xdecenter];
	double ydecenter = s.p[Pm_ydecenter];
	double   nradius = s.p[Pm_nradius];
	int      nzterms = s.p[Pm_nzterms];

	double tol = 0.0000000000001;

	Ray	Rn = r;

    //drays("This", &Rn, 1);
	for ( double iter = 0; iter < 5; iter++ ) {
	    double dz = zernike_std_value((Rn.p(X) + xdecenter)/nradius, (Rn.p(Y) + ydecenter)/nradius, nzterms, &s.p[Pm_z1]);
			zernike_std_slope((Rn.p(X) + xdecenter)/nradius, (Rn.p(Y) + ydecenter)/nradius, nzterms, &s.p[Pm_z1], &dx, &dy);

	    //fprintf(stderr, "X %f xde %f nrad %f\n", Rn.p(X), xdecenter, nradius);

	    //fprintf(stderr, "Here %f %f, %d %f\n", (Rn.p(X) + xdecenter)/nradius, (Rn.p(Y) + ydecenter)/nradius, nzterms, dz);
	    Rn.p += dz * Rn.k;					// move the ray to the estimate of the deformation surface.

	    if ( abs(dz) < tol ) { break; }
	}
    //drays("That", &Rn, 1);

	r = Rn;
    }

    // Normal
    //
    if ( R == 0.0 || abs(R) > 1.0e10 ) {	// Planar
	nhat = Vector3d(0.0, 0.0, -Dsign*1.0);
    } else {
	nhat = Vector3d(Rsign*Dsign*r.p(X), Rsign*Dsign*r.p(Y), -Dsign * sqrt(R * R - (K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y))));
	nhat /= nhat.norm();
    }


    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    //drays(&r, 1);

    return 0;
  }
}
