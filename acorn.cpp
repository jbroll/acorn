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
	    printf("%5d\t%10.4f\t%10.4f\t%10.4f\t", i, ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	        printf("%10.4f\t%10.4f\t%10.4f\t%d\n",    ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].vignetted);
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

    void trace_rays(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray)
    {
	int *traversed = new int[nray];

	for ( int h = 0; h < nsurfs; h++ ) {

	    Surface *surf = surflist[h].surf;
	    int     nsurf = surflist[h].nsurf;
	    int      once = surflist[h].type;

	    if ( once ) { for ( int j = 0; j < nray; j++ ) { traversed[j] = 0; } }

	    for ( int i = 0; i < nsurf; i++ ) {
		Affine3d transform;
		Affine3d inverse;

		transform 	= Translation3d(surf[i].x, surf[i].y, surf[i].z)
			    * AngleAxisd(d2r(surf[i].rx), Vector3d(1.0, 0.0, 0.0))
			    * AngleAxisd(d2r(surf[i].ry), Vector3d(0.0, 1.0, 0.0))
			    * AngleAxisd(d2r(surf[i].rz), Vector3d(0.0, 0.0, 1.0));

		inverse 	= transform.inverse();


		for ( int j = 0; j < nray; j++ ) {
		    Vector3d saveP = ray[j].p;
		    Vector3d saveK = ray[j].k;

			//printf("Ray  ");
			//prays(&ray[j], 1);

		    if ( ray[j].vignetted ) { continue; }

		    ray[j].p = transform * ray[j].p;		// Put the ray into the surface cs.
		    ray[j].k = transform * ray[j].k;

		    if ( (long) surf[i].traverse == COORDBK ) { continue; }


		    surf[i].traverse(n, z, &surf[i], &ray[j]);

		    ray[j].vignetted = aper_clip(&surf[i], &ray[j]);


		    if ( once ) {
			//printf("Here ");
			//prays(&ray[j], 1);

			  if ( !ray[j].vignetted ) {		// If the ray was not vignetted it has traversed this surface.
			//printf("Trav ");
			//prays(&ray[j], 1);
			      traversed[j] = 1;			// Don't try this ray again
			      ray[j].vignetted = 1;
			  } else {
			//printf("Ving ");
			//prays(&ray[j], 1);
			      ray[j].p = saveP;			// Reset
			      ray[j].k = saveK;

			      ray[j].vignetted = 0;		// Try again on next surface

			      continue;
			  }
		    }

		    ray[j].p = inverse * ray[j].p;		// Put the ray back into global cs..
		    ray[j].k = inverse * ray[j].k;

			//printf("Next ");
			//prays(&ray[j], 1);
		}

		if ( !once ) {
		    n  = surf[i].n > 0.0 ? surf[i].n : n;
		    z += surf[i].thickness;
		}
	    }
			//printf("Done ");
			//prays(&ray[0], 1);

	    if ( once ) {
		for ( int j = 0 ; j < nray; j++ ) {		// Rays that have not traversed are vignetted.
		    ray[j].vignetted = !traversed[j];
		}
		n  = surf[0].n > 0.0 ? surf[0].n : n;
		z += surf[0].thickness;
	    }
	}
	delete [] traversed;
    }
}

