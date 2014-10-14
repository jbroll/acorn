
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../Acorn.hh"
#include "../AcornSurface.hh"
#include "../AcornSurfGrp.hh"
#include "../AcornModel.hh"


#include "acorn-utils.hh"

struct AcornSurfaceTilted {
    ACORN_SURFACE

    Param double	tanx;
    Param double	tany;

    AcornSurfaceTilted();
};

static std::map<const char *, VarMap> VTable = {
#	include "tilted.vtable"
};

int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceTilted *s = (AcornSurfaceTilted *) S;

	double n0 = m->indicies[r.wave];
	double  z = m->z;

    double tanx = s->tanx;
    double tany = s->tany;
    double n    = s->indicies[r.wave];

    // http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-plane-and-ray-disk-intersection/

    Vector3d nhat = Vector3d(tanx, tany, 1.0).normalized();
    Vector3d l0   = -r.p;

    double d = nhat.dot(r.k);			// Denominator or Distance.

    if ( d < 1e-10 ) 	{ return 1; }

    d = l0.dot(nhat) / d; 

    if ( d < 0 ) 	{ return 1; };

    r.p += d * r.k; 				// Ray/Surface Intersection position

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
}

AcornSurfaceTilted::AcornSurfaceTilted() {
    tanx = 0;
    tany = 0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceTilted(); }
}
