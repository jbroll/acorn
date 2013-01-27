#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"


extern "C" {
    int  SurfSize() { return sizeof(Surface); }

    void trace_surfaces_seq(double z, double n, Surface *surf, int nsurf, Ray *ray, int nray)
    {
	for ( int i = 0; i < nsurf; i++ ) {

	    Affine3d transform;
	    Affine3d inverse;

	    transform 	= Translation3d(surf[i].x, surf[i].y, surf[i].z)
			* AngleAxisd(d2r(surf[i].rx), Vector3d(1.0, 0.0, 0.0))
			* AngleAxisd(d2r(surf[i].ry), Vector3d(0.0, 1.0, 0.0))
			* AngleAxisd(d2r(surf[i].rz), Vector3d(0.0, 0.0, 1.0));

	    inverse 	= transform.inverse();

	    for ( int j = 0; j < nray; j++ ) {
		if ( ray[j].vignetted ) { continue; }

		ray[j].p = transform * ray[j].p;

		  surf[i].traverse(n, z, &surf[i], &ray[j]);

		ray[j].p = inverse * ray[j].p;
	    }


	    n  = surf[i].n > 0.0 ? surf[i].n : n;
	    z += surf[i].thickness;
	}
    }

    void trace_surfaces_nsq(double z0, double n0, Surface *surfs, int nsurf, Ray *rays, int nray)
    {
	for ( int j = 0; j < nray; j++ ) {
	    if ( rays[j].vignetted ) { continue; }

	    double n = n0;
	    double z = z0;

	    for ( int i = 0; i < nsurf; i++ ) {
		surfs[i].traverse(n, z, &surfs[i], &rays[j]);

		if ( rays[j].vignetted ) {
		    rays[j].vignetted = 0;
		} else {
		    break;
		}

		n  = surfs[i].n > 0.0 ? surfs[i].n : n;
		z += surfs[i].thickness;
	    }
	}
    }
}

