
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


struct AcornSurfaceSimple {
    ACORN_SURFACE

    Param double	R;
    Param double	K;

    AcornSurfaceSimple();
};

static std::map<const char *, VarMap> VTable = {
#	include "simple.vtable"
};

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceSimple *s = (AcornSurfaceSimple *) S;

	double n0 = m->indicies[r.wave];
	double  z = m->z;

	double R = s->R;
	double K = s->K;
	double n = s->indicies[r.wave];

	Vector3d nhat;

    //
    double d = AcornSimpleSurfaceDistance(r, z, R, K);
    r.p += d * r.k; 				// Ray/Surface Intersection position

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
}

AcornSurfaceSimple::AcornSurfaceSimple() {
    R = 0;
    K = 0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceSimple(); }
}


