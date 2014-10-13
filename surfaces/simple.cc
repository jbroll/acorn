#include <stdio.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../Acorn.hh"
#include "../AcornUtils.hn"
#include "../AcornSurface.hh"

struct AcornSurfSimple {
    ACORN_SURFACE

    Param double	R;
    Param double	K;
}


int traverse(MData *m, AcornSurfSimple &s, Ray &r)
{
	double n0 = m->indicies[r.wave];
	double  z = m->z;

	double R = s.R;
	double K = s.K;
	double n = s.indicies[r.wave];

	Vector3d nhat;

    //
    d = AcornSimpleSurfaceDistance(r, z, R, K);
    r.p += d * r.k; 				// Ray/Surface Intersection position

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
}

extern "C" {
    AcornSurface *AcornSurfSimpleConstructor()   { return (AcornSurface *) new AcornSurfSimple(); }
}

