#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"


extern "C" {
#include <stdio.h>
#include "tcl.h"
#include "tpool/tpool.h"

    int  SurfSize() { return sizeof(Surface); }
    int  RaysSize() { return sizeof(Ray); }


    void prays(Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    printf("%5d\t%10.6f\t%10.6f\t%10.6f\t", i, ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	    printf("%10.6f\t%10.6f\t%10.6f\t%d\n",     ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].vignetted);
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

    void trace_rays0(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray)
    {
	int *traversed = new int[nray];

	for ( int h = 0; h < nsurfs; h++ ) {

	    Surface *surf = surflist[h].surf;
	    int     nsurf = surflist[h].nsurf;
	    int      once = surflist[h].type;

	    if ( once ) { for ( int j = 0; j < nray; j++ ) { traversed[j] = 0; } }

	    for ( int i = 0; i < nsurf; i++ ) {
		if ( isinf(surf[i].p[Px_thickness]) ) { continue; }

		Affine3d txforward;
		Affine3d txreverse;
		Affine3d rtforward;
		Affine3d rtreverse;

		txforward 	= Translation3d(-surf[i].p[Px_px], -surf[i].p[Px_py], -surf[i].p[Px_pz])
			    * AngleAxisd(d2r(surf[i].p[Px_rx]), Vector3d(1.0, 0.0, 0.0))
			    * AngleAxisd(d2r(surf[i].p[Px_ry]), Vector3d(0.0, 1.0, 0.0))
			    * AngleAxisd(d2r(surf[i].p[Px_rz]), Vector3d(0.0, 0.0, 1.0));

		rtforward 	=
			      AngleAxisd(d2r(surf[i].p[Px_rx]), Vector3d(1.0, 0.0, 0.0))
			    * AngleAxisd(d2r(surf[i].p[Px_ry]), Vector3d(0.0, 1.0, 0.0))
			    * AngleAxisd(d2r(surf[i].p[Px_rz]), Vector3d(0.0, 0.0, 1.0));

		txreverse 	= txforward.inverse();
		rtreverse 	= rtforward.inverse();

		//printf("Surface %d %d: %f %f %f\n", h, i, -surf[i].p[Px_px], -surf[i].p[Px_py], -surf[i].p[Px_pz]);

		for ( int j = 0; j < nray; j++ ) {
		    Vector3d saveP = ray[j].p;
		    Vector3d saveK = ray[j].k;

			//printf("Ray  ");
			//prays(&ray[j], 1);

		    if ( ray[j].vignetted ) { continue; }

		    ray[j].p = txforward * ray[j].p;		// Put the ray into the surface cs.
		    ray[j].k = rtforward * ray[j].k;

			//printf("Conv ");
			//prays(&ray[j], 1);

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

		    ray[j].p = txreverse * ray[j].p;		// Put the ray back into global cs..
		    ray[j].k = rtreverse * ray[j].k;

			//printf("Next ");
			//prays(&ray[j], 1);
		//if ( surf[i].z != 0 ) { prays(&ray[j], 1); }
		}

		if ( !once ) {
		    n  = surf[i].p[Px_n] > 0.0 ? surf[i].p[Px_n] : n;
		    z += surf[i].p[Px_thickness];
		}
	    }
			//printf("Done ");
			//prays(&ray[0], 1);

	    if ( once ) {
		for ( int j = 0 ; j < nray; j++ ) {		// Rays that have not traversed are vignetted.
		    ray[j].vignetted = !traversed[j];
		}
		n  = surf[0].p[Px_n] > 0.0 ? surf[0].p[Px_n] : n;
		z += surf[0].p[Px_thickness];
	    }
	}
	delete [] traversed;
    }
    typedef struct _TraceWork {
	double		 z;
	double		 n;
	SurfaceList 	*surflist;
	int		 nsurfs;
	Ray 		*ray;
	int		 nray;

	TPoolThread	*t;
    } TraceWork;

    void trace_rays_worker(TraceWork *work) {
	 trace_rays0(work->z, work->n, work->surflist, work->nsurfs, work->ray, work->nray);
    }
    void trace_rays_thread(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, void *tp, int nthread)
    {
	TraceWork data[64];


	if ( nthread == 0 ) {
	    trace_rays0(z, n, surflist, nsurfs, ray, nray);
	} else {
	    if ( tp == NULL ) { tp = TPoolInit(64); }

	    for ( int i = 0; i < nthread; i++ ) {
		data[i].z        = z;
		data[i].n        = n;
		data[i].surflist = surflist;
		data[i].nsurfs   = nsurfs;
		data[i].ray      = &ray[nray/nthread*i];
		data[i].nray     =      nray/nthread;

		if ( i == nthread-1 ) { data[i].nray = nray - nray/nthread*i; }

		data[i].t = TPoolThreadStart( (TPool*) tp, (TPoolWork) trace_rays_worker, &data[i]);
	    }
	    for ( int i = 0; i < nthread; i++ ) {
		TPoolThreadWait(data[i].t);
	    }
	}
    }

    TPool *tp = NULL;

    void trace_rays(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, int nthread) {
	 //trace_rays0(z, n, surflist, nsurfs, ray, nray);

    	trace_rays_thread(z, n, surflist, nsurfs, ray, nray, tp, nthread);
    }
}

