#include <iostream>
#include <stdio.h>
#include <math.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <map>
#include <vector>

#include "../Acorn.hh"
#include "../AcornSurface.hh"
#include "../AcornSurfGrp.hh"
#include "../AcornModel.hh"


#include "acorn-utils.hh" 


#define NASphere	8

struct AcornSurfaceEvenASphere {
    ACORN_SURFACE

    Param double	R;
    Param double	K;

    Param double	xdecenter;
    Param double	ydecenter;
    Param double	nradius;;
    Param int		nterms;;

    double	a[NASphere];

    AcornSurfaceEvenASphere();
};

static std::map<const char *, VarMap> VTable = {
#	include "evenasph.vtable"
};


    void EvenASphereSag(AcornSurface *S, double x, double y, double *dz, double *dx, double *dy)
    {
	AcornSurfaceEvenASphere *s = (AcornSurfaceEvenASphere *) S;

	int    i;
	int    nterms = max(s->nterms, NASphere);

	double adz = 0.0;
	double adx = 0.0;

	double  r = sqrt(x*x+y*y);
	double rr = 1;

	for ( i = 0; i < nterms; i++ ) {
	    rr = rr*r;					// rr is an odd.
	    adx = adx + (i+1)*2 * s->a[i] * rr;		// Make us a derrivitive.

	    rr = rr*r;					// rr is an even.
	    adz = adz +           s->a[i] * rr;		// Make us the function.
	}

	*dz = adz;
	*dx = adx/ r * x;					// Split slope on x,y.
	*dy = adx/ r * y;
    }

int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceEvenASphere *s = (AcornSurfaceEvenASphere *) S;

	double n0 = m->indicies[r.wave];
	double  z = m->z;

    double R 	  = s->R;
    double K 	  = s->K;
    double n 	  = s->indicies[r.wave];
    int    nterms = s->nterms;

    Vector3d nhat;


    if ( nterms ) {
	if ( AcornSimpleIterativeIntersect(R, K, n, (AcornSurface &) s, r, z, nhat, EvenASphereSag) ) { return 1; }
    } else {
	double d = AcornSimpleSurfaceDistance(r, z, R, K); 		// Ray/Surface intersection position
	r.p += d * r.k;

	nhat = AcornSimpleSurfaceNormal(r, R, K);
    }

    AcornRefract(r, nhat, n0, n);				// Reflect or Refract

    return 0;
}

AcornSurfaceEvenASphere::AcornSurfaceEvenASphere() {
    R 		= 0.0;
    K 		= 0.0;
    xdecenter   = 0.0;
    ydecenter   = 0.0;
    nradius	= 0.0;

    nterms	= 0;

    for ( int i = 0; i < NASphere; i++ ) { a[i] = 0; }

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceEvenASphere(); }
}

__attribute__((constructor)) static void AcornSurfaceEvenASphereInit() {
    VTable["a2"]  = { Type_double, offsetof(AcornSurfaceEvenASphere, a[0]) };
    VTable["a4"]  = { Type_double, offsetof(AcornSurfaceEvenASphere, a[1]) };
    VTable["a6"]  = { Type_double, offsetof(AcornSurfaceEvenASphere, a[2]) };
    VTable["a8"]  = { Type_double, offsetof(AcornSurfaceEvenASphere, a[3]) };
    VTable["a10"] = { Type_double, offsetof(AcornSurfaceEvenASphere, a[4]) };
    VTable["a12"] = { Type_double, offsetof(AcornSurfaceEvenASphere, a[5]) };
    VTable["a14"] = { Type_double, offsetof(AcornSurfaceEvenASphere, a[6]) };
    VTable["a16"] = { Type_double, offsetof(AcornSurfaceEvenASphere, a[7]) };
}

