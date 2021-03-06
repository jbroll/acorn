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
	    printf("%10.6f\t%10.6f\t%10.6f\t%d\t%d\n",     ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].wave, ray[i].vignetted);
	}
    }

    void trace_rays0(MData *m, SurfaceList *surflist, int nsurfs, Ray *R, int nray, int rsize, char *xray)
    {

	int *traversed = new int[nray];
	Ray *ray;

	//fprintf(stderr, "Ray0 %ld %p %d %d\n", R, R, nray, rsize);
	//fflush(stderr);

	//return;

	for ( int h = 0; h < nsurfs; h++ ) {

	    Surface *surf = surflist[h].surf;
	    int     nsurf = surflist[h].nsurf;
	    int      once = surflist[h].type;
	    int		j;

	    if ( once ) { for ( int j = 0; j < nray; j++ ) { traversed[j] = 0; } }

	    for ( int i = 0; i < nsurf; i++ ) {
		if ( isinf(surf[i].p[Px_thickness]) ) { continue; }
		if (      !surf[i].enable           ) { continue; } 

		Affine3d txforward = Affine3d::Identity();
		Affine3d txreverse  ;
		Affine3d rtforward  ;
		Affine3d rtreverse  ;

		txforward   = 						// Ray position transform
			    Affine3d::Identity()
			    * Translation3d(0.0, 0.0,  m->z)
				* AngleAxisd(d2r( surf[i].p[Px_rz]), Vector3d(0.0, 0.0, -1.0))
				* AngleAxisd(d2r(-surf[i].p[Px_ry]), Vector3d(0.0, 1.0,  0.0))
				* AngleAxisd(d2r(-surf[i].p[Px_rx]), Vector3d(1.0, 0.0,  0.0))
			    * Translation3d(0.0, 0.0, -m->z)
			    * Translation3d(-surf[i].p[Px_px], -surf[i].p[Px_py], -surf[i].p[Px_pz])
			;

		rtforward   =						// Ray direction transform
			    Affine3d::Identity()
				* AngleAxisd(d2r( surf[i].p[Px_rz]), Vector3d(0.0, 0.0, -1.0))
				* AngleAxisd(d2r(-surf[i].p[Px_ry]), Vector3d(0.0, 1.0,  0.0))
				* AngleAxisd(d2r(-surf[i].p[Px_rx]), Vector3d(1.0, 0.0,  0.0))
			;

		txreverse 	= txforward.inverse();
		rtreverse 	= rtforward.inverse();

		//printf("Surface %s %d %d: %f %f %f	%d %ld\n", surf[i].type, h, i, -surf[i].p[Px_px], -surf[i].p[Px_py], -surf[i].p[Px_pz], once, surf[i].traverse);

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

		    ray->vignetted = surf[i].traverse(m, &surf[i], ray);

			//printf("Trav ");
			//prays(ray, 1);

		    if ( ray->vignetted == 2 ) {		// Coordbreak returns 2
			ray->vignetted = 0;

			if ( xray ) {
			    memcpy(xray, ray, rsize);
			    xray += rsize;
			}
			continue; 
		    }

		    if ( ray->vignetted || (!ray->vignetted && aper_clip(&surf[i], ray)) ) {
		        ray->vignetted = i ? i : -1;
		    }

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

		    if ( xray ) {
			memcpy(xray, ray, rsize);
			xray += rsize;
		    }

			//printf("Next ");
			//prays(ray, 1);
		}

		if ( !once ) {
		    m->indicies  = surf[i].indicies[0] > 0.0 ? surf[i].indicies: m->indicies;
		    m->z += surf[i].p[Px_thickness];
		}
	    }
			//printf("Done ");
			//prays(ray, 1);

	    if ( once ) {
		for ( j = 0, ray = R; j < nray; j++, ray = (Ray *) (((char *) ray) + rsize) ) {	// Rays that have not traversed are vignetted.
		    ray->vignetted = !traversed[j];
		}
		m->indicies  = surf[0].indicies[0] > 0.0 ? surf[0].indicies: m->indicies;
		m->z += surf[0].p[Px_thickness];
	    }
	}
	delete [] traversed;
    }

    typedef struct _TraceWork {
	MData		 m;
	SurfaceList 	*surflist;
	int		 nsurfs;
	Ray 		*ray;
	int		 nray;
	int		rsize;
	char		*xray;

	TPoolThread	*t;
    } TraceWork;

    void trace_rays_worker(TraceWork *work) {
	 trace_rays0(&work->m, work->surflist, work->nsurfs, work->ray, work->nray, work->rsize, work->xray);
    }
    void trace_rays_thread(MData *m, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, int rsize, void *tp, int nthread, char *xray)
    {
	TraceWork data[64];


	if ( nthread == 0 ) {
	    trace_rays0(m, surflist, nsurfs, ray, nray, rsize, xray);
	} else {
	    for ( int i = 0; i < nthread; i++ ) {
		data[i].m        = *m;
		data[i].surflist = surflist;
		data[i].nsurfs   = nsurfs;
		data[i].ray      = (Ray *)((char *) ray + (nray/nthread)*i*rsize);
		data[i].nray     =      nray/nthread;
		data[i].rsize    = rsize;
		if ( xray ) {
		    data[i].xray = xray + (nray/nthread)*i*rsize;
		} else {
		    data[i].xray = NULL;
		}

		if ( i == nthread-1 ) { data[i].nray = nray - nray/nthread*i; }

		data[i].t = TPoolThreadStart( (TPool*) tp, (TPoolWork) trace_rays_worker, &data[i]);
	    }
	    for ( int i = 0; i < nthread; i++ ) {
		TPoolThreadWait(data[i].t);
	    }
	}
    }

    TPool *tp = NULL;

    void trace_rays(MData *m, SurfaceList *surflist, int nsurfs, Ray *ray, int nray, int rsize, int nthread, char *xray) {
	if ( tp == NULL ) {
	    tp = TPoolInit(128);
	}

    	trace_rays_thread(m, surflist, nsurfs, ray, nray, rsize, tp, nthread, xray);
    }
}

