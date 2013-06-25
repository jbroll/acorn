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

#if 0
	// Diffract

	double dpdy = M*T*L;		// diffraction routine from Welford
	double nn;
	double rad;

	/* account for possible change in index or mirrors */

	dpdy /= fabs(n);
	if ( n0*n < 0 ) { dpdy = -dpdy; }
	if ( n0   < 0 ) {  nn = -1.0;
	} else	 	{  nn =  1.0; }

	double nx = -nhat->x;
	double ny = -nhat->y;
	double nz = -nhat->z;

	double ux = r->kx + nn * (dpdx);
	double uy = r->ky + nn * (dpdy);
	double uz = r->kz;

	rad = nx*ux + ny*uy + nz*uz;
	rad = 1.0 - (ux*ux + uy*uy + uz*uz) + rad*rad;

	if ( rad <= 0.0 ) { rad = 0.0; 
	} else 		  { rad = sqrt(rad); }

	r.kx = ux - (nx*ux + ny*uy + nz*uz)*nx + nx * rad;
	r.ky = uy - (nx*ux + ny*uy + nz*uz)*ny + ny * rad;
	r.kz = uz - (nx*ux + ny*uy + nz*uz)*nz + nz * rad;
#endif

	return 0;
    }
}
