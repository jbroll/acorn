#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <stdio.h>

#include "../acorn.h"

    enum Px_Local { Pm_R = Px_NParams, Pm_K, Pm_nterms, Pm_a2 };

#include "acorn-utils.h"


extern "C" {

  static const char  *MyParamNames[] = { "R", "K", "nterms", "a2", "a4", "a6", "a8", "a10", "a12", "a14", "a16" };
  static const double MyParamValue[] = { 0.0, 0.0,      7.0,  0.0,  0.0,  0.0,  0.0,   0.0,   0.0,   0.0,   0.0 };


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

    void EvenASpSag(Surface &s, double x, double y, double *dz, double *dx, double *dy)
    {
	int    i;
	int    nterms = (int) max(s.p[Pm_nterms], 8.0);

	double adz = 0.0;
	double adx = 0.0;

	double  r = sqrt(x*x+y*y);
	double rr = 1;

	for ( i = 0; i < nterms; i++ ) {
	    rr = rr*r;						// rr is an odd.
	    adx = adx + (i+1)*2 * s.p[Pm_a2+i] * rr;		// Make us a derrivitive.

	    rr = rr*r;						// rr is an even.
	    adz = adz +           s.p[Pm_a2+i] * rr;		// Make us the function.
	}

	*dz = adz;
	*dx = adx/ r * x;					// Split slope on x,y.
	*dy = adx/ r * y;
    }

  int traverse(MData *m, Surface &s, Ray &r)
  {
	double n0 = m->n;
	double  z = m->z;

    double d;

    double R = s.p[Pm_R];
    double K = s.p[Pm_K];
    double n = s.p[Px_n];

    Vector3d nhat;

    int      nterms = (int) s.p[Pm_nterms];

    if ( nterms ) {
	if ( AcornSimpleIterativeIntersect(R, K, n, s, r, z, nhat, EvenASpSag) ) { return 1; }
    } else {
	d = AcornSimpleSurfaceDistance(r, z, R, K); 		// Ray/Surface intersection position
	r.p += d * r.k;

	nhat = AcornSimpleSurfaceNormal(r, R, K);
    }

    AcornRefract(r, nhat, n0, n);				// Reflect or Refract

    return 0;
  }
}
