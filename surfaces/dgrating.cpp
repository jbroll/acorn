#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <stdio.h>

#include "../acorn.h"

    enum Px_Local { Pm_R = Px_NParams, Pm_K, Pm_lines, Pm_order };

#include "acorn-utils.h"


extern "C" {

  static const char  *MyParamNames[] = { "R", "K", "l/mm", "order" };
  static const double MyParamValue[] = { 0.0, 0.0, 0.0, 1.0 };


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

    int traverse(MData *m, Surface &s, Ray &r)
    {
	double n0 = m->n;
	double  z = m->z;

	double d;

	double R = s.p[Pm_R];
	double K = s.p[Pm_K];
	double n = s.p[Px_n];

	Vector3d nhat;

	// Intersect
	//
	d = AcornSimpleSurfaceDistance(r, z, R, K);

	// Ray/Surface Intersection position
	//
	r.p += d * r.k;

	nhat = AcornSimpleSurfaceNormal(r, R, K);

	AcornRefract(r, nhat, n0, n);		// Reflect or Refract

	double T = s.p[Pm_lines];
	double M = s.p[Pm_order];
	double L = m->w;

#if 1
	// Diffract

	double dpdy = M*T*L/10000.0;		// diffraction routine from Welford
	double dpdx = 0;
	double nn   = 1.0;
	double rad;

	/* account for possible change in index or mirrors */

	dpdy /= fabs(n);
//	if ( n0*n < 0 ) { dpdy = -dpdy; }
//	if ( n0   < 0 ) {  nn = -1.0;
//	} else	 	{  nn =  1.0; }

	double nx = nhat(X);
	double ny = nhat(Y);
	double nz = nhat(Z);

	double ux = r.k(X) + nn * (dpdx);
	double uy = r.k(Y) + nn * (dpdy);
	double uz = r.k(Z);

	rad = nx*ux + ny*uy + nz*uz;
	rad = 1.0 - (ux*ux + uy*uy + uz*uz) + rad*rad;

	if ( rad <= 0.0 ) { rad = 0.0; 
	} else 		  { rad = sqrt(rad); }

	r.k(X) = ux - (nx*ux + ny*uy + nz*uz)*nx + nx * rad;
	r.k(Y) = uy - (nx*ux + ny*uy + nz*uz)*ny + ny * rad;
	r.k(Z) = uz - (nx*ux + ny*uy + nz*uz)*nz + nz * rad;
#endif

	return 0;
    }
}
