#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"

extern "C" {
#include <stdio.h>
#include <string.h>

    // Copyright (c) 1970-2003, Wm. Randolph Franklin	: http://www.ecse.rpi.edu/~wrf/Research/Short_Notes/pnpoly.html
    //
    int pnpoly(int nvert, Vector3d *vert, double x, double y)
    {
	int i, j, c = 0;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
	    if ( ((vert[i](Y) > y) != (vert[j](Y) > y)) &&
                 (x < (vert[j](X)-vert[i](X)) * (y-vert[i](Y)) / (vert[j](Y)-vert[i](Y)) + vert[i](X)) )
		c = !c;
	}
	return c;
    }

    void aper_init(Surface *s, Affine3d transform)
    {
	int i;


	if ( s->aper_type ) {
	    if ( !strcmp(s->aper_type, "circular") ) { return; }
	    if ( !strcmp(s->aper_type, "annulus") )  { return; }
	    if ( !strcmp(s->aper_type, "UDA") )      {
		if (  s->aper_priv ) { free((void *) s->aper_priv);  s->aper_priv = 0; }
		if ( !s->aper_priv ) { s->aper_priv = (long) malloc(s->aper_leng * sizeof(Vector3d)); }

		Vector3d *aper_data = (Vector3d *) s->aper_data;
		Vector3d *aper_priv = (Vector3d *) s->aper_priv;

		for ( i = 0; i < s->aper_leng; i++ ) {
		    aper_priv[i] = transform * aper_data[i];
		}
	    }
	}
	fflush(stdout);
    }

    int aper_clip(Surface *s, Ray *r)
    {
	if ( s->aper_type ) {
	    if ( !strcmp(s->aper_type, "circular") ) {
		return r->p(X)*r->p(X)+r->p(Y)*r->p(Y) > s->aper_max*s->aper_max;
	    }
	    if ( !strcmp(s->aper_type, "annulus") )  {
		return r->p(X)*r->p(X)+r->p(Y)*r->p(Y) > s->aper_max*s->aper_max
		    || r->p(X)*r->p(X)+r->p(Y)*r->p(Y) < s->aper_max*s->aper_min ;
	    }

	    if ( !strcmp(s->aper_type, "UDA") && s->aper_leng )      { return !pnpoly(s->aper_leng, (Vector3d *)s->aper_data, r->p[X], r->p[Y]); }
	}

	fflush(stdout);
	return 0;
    }
}
