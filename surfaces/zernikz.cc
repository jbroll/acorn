#include <iostream>
#include <stdio.h>
#include <math.h>
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

extern "C" {
#   include "../zernike/zernike.h"
}

#define NZernike	104

struct AcornSurfaceZernike {
    ACORN_SURFACE

    Param double	R;
    Param double	K;

    Param double	xdecenter;
    Param double	ydecenter;
    Param double	nradius;;
    Param int		nterms;;

    double		z[NZernike];

    AcornSurfaceZernike();
};

static CStrVarMapMap VTable = {
#	include "zernikz.vtable"
};


    void ZernikeSag(AcornSurface *S, double x, double y, double *dz, double *dx, double *dy)
    {
	AcornSurfaceZernike *s = (AcornSurfaceZernike *) S;

	double xdecenter = s->xdecenter;
	double ydecenter = s->ydecenter;
	double   nradius = s->nradius;
	int      nzterms = s->nterms;

	double zdz, zdx, zdy;

	zernikz_std((x - xdecenter)/nradius, (y - ydecenter)/nradius, nzterms, s->z, &zdz, &zdx, &zdy);

	zdx /= nradius;
	zdy /= nradius;

	*dz = zdz;
	*dx = zdx;
	*dy = zdy;
    }

int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceZernike *s = (AcornSurfaceZernike *) S;

	double n0 = m->indicies[r.wave];
	double  z = m->z;

    double R 	  = s->R;
    double K 	  = s->K;
    double n 	  = s->indicies[r.wave];
    int    nterms = s->nterms;

    Vector3d nhat;


    if ( nterms ) {
	if ( AcornSimpleIterativeIntersect(R, K, n, (AcornSurface &) s, r, z, nhat, ZernikeSag) ) { return 1; }
    } else {
	double d = AcornSimpleSurfaceDistance(r, z, R, K); 		// Ray/Surface intersection position
	r.p += d * r.k;

	nhat = AcornSimpleSurfaceNormal(r, R, K);
    }

    AcornRefract(r, nhat, n0, n);				// Reflect or Refract

    return 0;
}

AcornSurfaceZernike::AcornSurfaceZernike() {
    R 		= 0.0;
    K 		= 0.0;
    xdecenter   = 0.0;
    ydecenter   = 0.0;
    nradius	= 0.0;

    nterms	= 0;

    for ( int i = 0; i < NZernike; i++ ) { z[i] = 0; }

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceZernike(); }
}

__attribute__((constructor)) static void AcornSurfaceZernikzInit() {

    for ( int i = 0; i < NZernike; i++ ) {
	char *name = (char *) malloc(10);

	sprintf(name, "z%d", i);

	VTable[name]  = VarMap(Type_double, offsetof(AcornSurfaceZernike, z)+1);
    }
}
