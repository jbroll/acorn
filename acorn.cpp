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

    void trace_rays0(double z, double n, SurfaceList *surflist, int nsurfs, Ray *R, int nray, int rsize)
    {
	int *traversed = new int[nray];
	Ray *ray;

	for ( int h = 0; h < nsurfs; h++ ) {

	    Surface *surf = surflist[h].surf;
	    int     nsurf = surflist[h].nsurf;
	    int      once = surflist[h].type;
	    int		j;

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

		for ( j = 0, ray = R; j < nray; j++, ray = (Ray *) (((char *) ray) + rsize) ) {
		    Vector3d saveP = ray->p;
		    Vector3d saveK = ray->k;

			//printf("Ray  ");
			//prays(ray, 1);

		    if ( ray->vignetted ) { continue; }

		    ray->p = txforward * ray->p;		// Put the ray into the surface cs.
		    ray->k = rtforward * ray->k;

			//printf("Conv ");
			//prays(ray, 1);

		    if ( (long) surf[i].traverse == COORDBK ) { continue; }


		    //printf("traverse %f %f\n", n, z);
		    ray->vignetted = surf[i].traverse(n, z, &surf[i], ray);


		    if ( once ) {
			//printf("Here ");
			//prays(ray, 1);

			  if ( !ray->vignetted ) {		// If the ray was not vignetted it has traversed this surface.
			//printf("Trav ");
			//prays(ray, 1);
			      traversed[j] = 1;			// Don't try this ray again
			      ray->vignetted = 1;
			  } else {
			//printf("Ving ");
			//prays(&ray[j], 1);
			      ray->p = saveP;			// Reset
			      ray->k = saveK;

			      ray->vignetted = 0;		// Try again on next surface

			      continue;
			  }
		    }

		    ray->p = txreverse * ray->p;		// Put the ray back into global cs..
		    ray->k = rtreverse * ray->k;

			//printf("Next ");
			//prays(ray, 1);
		//if ( surf[i].z != 0 ) { prays(ray, 1); }
		}

		if ( !once ) {
		    n  = surf[i].p[Px_n] > 0.0 ? surf[i].p[Px_n] : n;
		    z += surf[i].p[Px_thickness];
		}
	    }
			//printf("Done ");
			//prays(ray, 1);

	    if ( once ) {
		for ( int j = 0 ; j < nray; j++ ) {		// Rays that have not traversed are vignetted.
		    ray->vignetted = !traversed[j];
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
	int		rsize;

	TPoolThread	*t;
    } TraceWork;

    void trace_rays_worker(TraceWork *work) {
	 trace_rays0(work->z, work->n, work->surflist, work->nsurfs, work->ray, work->nray, work->rsize);
    }
    void trace_rays_thread(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, int rsize, void *tp, int nthread)
    {
	TraceWork data[64];

	if ( nthread == 0 ) {
	    trace_rays0(z, n, surflist, nsurfs, ray, nray, rsize);
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

    void trace_rays(double z, double n, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, int rsize, int nthread) {
	 //trace_rays0(z, n, surflist, nsurfs, ray, nray);

    	trace_rays_thread(z, n, surflist, nsurfs, ray, nray, rsize, tp, nthread);
    }
}

