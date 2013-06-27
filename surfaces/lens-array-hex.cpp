#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"

  enum Px_Local { Pm_R = Px_NParams, Pm_K, Pm_nx, Pm_ny, Pm_width };

#include "acorn-utils.h"


extern "C" {

  static const char  *MyParamsNames[] = { "R", "K", "nx", "ny", "width", "height" };
  static const double MyParamsValue[] = { 0.0, 0.0, 10, 10, 1.0, 1.0 };
  static const char  *MyAnnote[] = { "double cx", "double cy" };

  int info(int command, char **strings, void **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {

	    *strings = (char *)   MyParamsNames;
	    *values  = (double *) MyParamsValue;

	    return sizeof(MyParamsNames)/sizeof(char *);
        }
	case ACORN_STRINGS: { return 0; }
        case ACORN_ANNOTE:  {
	    *strings = (char **) MyAnnote;
	    return 1; 
	}
    }
    return 0;
  }


    int LensCenter(int nx, int ny, double wx, double x, double y, double *cx, double *cy)
    {
	double p = wx;
	double r = p / sqrt(3.);  /* Distance from center to corner of hexagon */
	double h = 1.5 * r;  /*  h = distance between rows */

	double xc, yc;
	double dx, dy;

	int row =  (int) round(y / h);
	int col = (int) (row % 2 == 0 ? round(x/p) : round(x/p + 0.5));

	yc = h * row;
	xc = p * (col - (row % 2 != 0) / 2.);


	/* Check to see if we're in a corner */
	dx = fabs(x - xc);
	dy = fabs(y - yc);

	if (dy > r - dx * r / p) {
	    col += ((x > xc) ? 1 : 0) - ((row % 2) ? 1 : 0) ;
	    row += ((y > yc) ? 1 : -1);
	}

	*cx = h * row;
	*cy = p * (col - (row % 2 != 0) / 2.);

	if ( fabs(*cx) > (nx*0.5)*p ) return -1;
	if ( fabs(*cy) > (ny*0.5)*h ) return -1;

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

    // Intersect
    //
    // establish sign flippy dealies
    //
    double Ksign = 1.0;
    double Dsign = r.k(Z)/fabs(r.k(Z));
    double Rsign = R/fabs(R);

    Vector3d nhat;

    double cx = 0, cy = 0;


    if ( R == 0.0 )  {			// Planar
	d = (z - r.p(Z))/r.k(Z);
    } else {					// http://www-physics.ucsd.edu/~tmurphy/astr597/exercises/raytrace-3d.pdf

	if ( LensCenter((int) s.p[Pm_nx], (int) s.p[Pm_nx], s.p[Pm_width], r.p(X), r.p(Y), &cx, &cy) ) { return 1; }

	//printf("Center %f %f	: %f %f\n", r.p(X), r.p(Y), cx, cy);
	r.p(X) -= cx;
	r.p(Y) -= cy;

	d = AcornSimpleSurfaceDistance(r, z, R, K);
    }

    // Ray/Surface Intersection position
    //
    r.p += d * r.k;

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    r.p(X) += cx;
    r.p(Y) += cy;

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    if ( s.annote ) {
	double *here = (double *) (((char *) &r) + s.annote);

	here[0] = cx;
	here[1] = cy;
    }

    return 0;
  }
}
