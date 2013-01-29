#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"


extern "C" {
#include <stdio.h>

    int  SurfSize() { return sizeof(Surface); }
    int  RaysSize() { return sizeof(Ray); }


    void prays(Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    printf("%10.5f %10.5f %10.5f\t", ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	    printf("%10.5f %10.5f %10.5f\n", ray[i].k(X), ray[i].k(Y), ray[i].k(Z));
	}
    }
    void xrays(Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    printf("%p	%016llx %016llx %016llx\t", &ray[i]
		    					, ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	    printf("%016llx %016llx %016llx\n" , ray[i].k(X), ray[i].k(Y), ray[i].k(Z));
	}
    }

    void trace_surfaces(double z, double n, Surface *surf, int nsurf, Ray *ray, int nray, int once)
    {
	int *traversed;

	if ( once ) {					// Allow a ray to traverse only a single surface.
	    traversed = new int[nray];
	}

	for ( int i = 0; i < nsurf; i++ ) {

	    Affine3d transform;
	    Affine3d inverse;


	    transform 	= Translation3d(surf[i].x, surf[i].y, surf[i].z)
			* AngleAxisd(d2r(surf[i].rx), Vector3d(1.0, 0.0, 0.0))
			* AngleAxisd(d2r(surf[i].ry), Vector3d(0.0, 1.0, 0.0))
			* AngleAxisd(d2r(surf[i].rz), Vector3d(0.0, 0.0, 1.0));

	    inverse 	= transform.inverse();

	    for ( int j = 0; j < nray; j++ ) {
		if ( ray[j].vignetted || once && traversed[j] ) { continue; }

		ray[j].p = transform * ray[j].p;	// Put the ray into the surface cs.
		ray[j].k = transform * ray[j].k;

		  surf[i].traverse(n, z, &surf[i], &ray[j]);

		  if ( once ) {
		      if ( !ray[i].vignetted ) {	// If the ray was not vignetted it has traversed this surface.
			traversed[i] = 1;		// Don't try this ray again
		      } 
		      ray[i].vignetted = 0;
		  }

		ray[j].p = inverse * ray[j].p;		// Put the ray back into global cs..
		ray[j].k = inverse * ray[j].k;
	    }


	    n  = surf[i].n > 0.0 ? surf[i].n : n;
	    z += surf[i].thickness;
	}

	if ( once ) {
	    for ( int i = 0 ; i < nray; i++ ) {		// Rays that have not traversed are vignetted.
		ray[i].vignetted = !traversed[i];
	    }
	    delete [] traversed;
	}
    }
}

