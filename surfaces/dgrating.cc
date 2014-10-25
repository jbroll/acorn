#include <stdio.h>
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


struct AcornSurfaceDGrating {
    ACORN_SURFACE

    Param double	R;
    Param double	K;
    Param double	l_mm;
    Param double	order;

    AcornSurfaceDGrating();
};

#define MyClass AcornSurfaceDGrating

static CStrVarMapMap VTable = {
#	include "dgrating.vtable"
};

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceDGrating *s = (AcornSurfaceDGrating *) S;

    double n0 = m->indicies[r.wave];
    double  z = m->z;

    double d;

    double R = s->R;
    double K = s->K;
    double n = s->indicies[r.wave];

    Vector3d nhat;

    // Intersect
    //
    d = AcornSimpleSurfaceDistance(r, z, R, K);

    // Ray/Surface Intersection position
    //
    r.p += d * r.k;

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    double T = s->l_mm;
    double M = s->order;
    double L = m->wavelength[r.wave];


    // Diffract

    double dpdy = M*T*L/10000.0;		// diffraction routine from Welford
    double dpdx = 0.0;
    double nn   = 1.0;
    double rad;

    /* account for possible change in index or mirrors */

    dpdy /= fabs(n);

    //if ( n0*n < 0 ) { dpdy = -dpdy; }
    if ( n0   < 0 ) {  nn = -1.0;
    } else	 	{  nn =  1.0; }

    double ux = r.k(X) + nn * (dpdx);
    double uy = r.k(Y) + nn * (dpdy);
    double uz = r.k(Z);

    Vector3d U = Vector3d(ux, uy, uz);

    rad = nhat.dot(U); 					//rad = nx*ux + ny*uy + nz*uz;
    rad = 1.0 - U.dot(U) + rad*rad; 			//rad = 1.0 - (ux*ux + uy*uy + uz*uz) + rad*rad;
    
    if ( rad <= 0.0 ) { rad = 0.0; 
    } else 		  { rad = sqrt(rad); }

							    //r.k(X) = ux - (nx*ux + ny*uy + nz*uz)*nx + nx * rad;
							    //r.k(Y) = uy - (nx*ux + ny*uy + nz*uz)*ny + ny * rad;
							    //r.k(Z) = uz - (nx*ux + ny*uy + nz*uz)*nz + nz * rad;
    r.k = U.array() - nhat.dot(U) * nhat.array() + nhat.array() * rad;

    return 0;
}

AcornSurfaceDGrating::AcornSurfaceDGrating() {
    R		= 0.0;
    K		= 0.0;
    l_mm 	= 0.0;
    order 	= 1.0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceDGrating(); }
}
