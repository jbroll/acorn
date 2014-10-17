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

struct AcornSurfaceCoordBrk {
    ACORN_SURFACE

    Param double R;
    Param double K;

    AcornSurfaceCoordBrk();
};

static CStrVarMapMap VTable = {
#	include "coordbrk.vtable"
};

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
    AcornSurfaceCoordBrk *s = (AcornSurfaceCoordBrk *) S;

    double	z = m->z;
    double 	R = s->R;
    double	K = s->K;

    double d = AcornSimpleSurfaceDistance(r, z, R, K);
    r.p += d * r.k; 					// Ray/Surface Intersection position

    return 2;
}

AcornSurfaceCoordBrk::AcornSurfaceCoordBrk() {
    R = 0;
    K = 0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}


extern "C" {
    AcornSurface *AcornSurfConstructor() { return (AcornSurface *) new AcornSurfaceCoordBrk(); }
}

