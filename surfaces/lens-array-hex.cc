#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../Acorn.hh"
#include "../AcornSurface.hh"
#include "../AcornSurfGrp.hh"
#include "../AcornModel.hh"

#include "acorn-utils.hh"

struct AcornSurfaceLensArrayHex {
    ACORN_SURFACE

    Param double	R;
    Param double	K;
    Param double	nx;
    Param double	ny;
    Param double	width;

    AcornSurfaceLensArrayHex();
};

static std::map<const char *, VarMap> VTable = {
#	include "lens-array-hex.vtable"
};


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

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceLensArrayHex *s = (AcornSurfaceLensArrayHex *) S;

	double n0 = m->indicies[r.wave];
	double  z = m->z;

    double d;

    double R = s->R;
    double K = s->K;
    double n = s->indicies[r.wave];

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

	if ( LensCenter(s->nx, s->nx, s->width, r.p(X), r.p(Y), &cx, &cy) ) { return 1; }

	//printf("Center %f %f	: %f %f\n", r.p(X), r.p(Y), cx, cy);
	r.p(X) -= cx;
	r.p(Y) -= cy;

	d = AcornSimpleSurfaceDistance(r, z, R, K);
    }

    // Ray/Surface Intersection position
    //
    r.p += d * r.k;


    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    r.p(X) += cx;
    r.p(Y) += cy;

    if ( s->annote ) {
	double *here = (double *) (((char *) &r) + s->annote);

	here[0] = cx;
	here[1] = cy;
    }

    return 0;
}

AcornSurfaceLensArrayHex::AcornSurfaceLensArrayHex() {
    R = 0;
    K = 0;
    nx = 0;
    ny = 0;
    width = 0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceLensArrayHex(); }
}

