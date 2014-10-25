#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <map>
#include <vector>

#include "../Acorn.hh"
#include "../AcornRay.hh"
#include "../AcornSurface.hh"
#include "../AcornSurfGrp.hh"
#include "../AcornModel.hh"

#include "acorn-utils.hh"

struct AcornSurfaceLensArrayRect {
    ACORN_SURFACE

    Param double	R;
    Param double	K;
    Param double	nx;
    Param double	ny;
    Param double	width;

    AcornSurfaceLensArrayRect();
};

#define MyClass AcornSurfaceLensArrayRect

static CStrVarMapMap VTable = {
#	include "lens-array-rect.vtable"
};


    int LensCenter(int nx, int ny, double wx, double x, double y, double *cx, double *cy)
    {
	*cx = x / wx;
	*cy = y / wx;

	if ( x > 0 ) { x = floor(*cx + 0.5); } else { x =  ceil(*cx - 0.5); }
	if ( y > 0 ) { y = floor(*cy + 0.5); } else { y =  ceil(*cy - 0.5); }

	*cx = x * wx;
	*cy = y * wx;

	if ( *cx > (nx-0.5)*wx ) return -1;
	if ( *cy > (ny-0.5)*wx ) return -1;

	return 0;
    }

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceLensArrayRect *s = (AcornSurfaceLensArrayRect *) S;

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

AcornSurfaceLensArrayRect::AcornSurfaceLensArrayRect() {
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
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceLensArrayRect(); }
}

