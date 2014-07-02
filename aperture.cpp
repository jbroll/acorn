#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "acorn.h"

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

    int aper_clip(Surface *s, Ray *r)
    {
	if ( s->aper_type && *s->aper_type ) {

	    double px = r->p(X) - s->p[Px_aper_xoff];
	    double py = r->p(Y) - s->p[Px_aper_yoff];

	    if ( !strcmp(s->aper_type, "circular") ) {
		return r->p(X)*r->p(X)+r->p(Y)*r->p(Y) > s->p[Px_aper_max]*s->p[Px_aper_max];
		return px*px+py*py > s->p[Px_aper_max]*s->p[Px_aper_max];
	    }
/*
	    if ( !strcmp(s->aper_type, "rectangular") ) {
		double w = s->p[Px_aper_min];
		double h = s->p[Px_aper_max];

		return px < -w || px > w || py < -h || py > h;
	    }
 */
	    if ( !strcmp(s->aper_type, "obstruction") ) {
		return r->p(X)*r->p(X)+r->p(Y)*r->p(Y) < s->p[Px_aper_min]*s->p[Px_aper_min];
	    }

	    if ( !strcmp(s->aper_type, "annulus") )  {
                return r->p(X)*r->p(X)+r->p(Y)*r->p(Y) > s->p[Px_aper_max]*s->p[Px_aper_max]
                   || r->p(X)*r->p(X)+r->p(Y)*r->p(Y) < s->p[Px_aper_max]*s->p[Px_aper_min] ;

		return px*px+py*py > s->p[Px_aper_max]*s->p[Px_aper_max]
		    || px*px+py*py < s->p[Px_aper_max]*s->p[Px_aper_min] ;
	    }

	    if ( !strcmp(s->aper_type, "UDA") && s->aper_leng ) {
		return !pnpoly(s->aper_leng, (Vector3d *)s->aper_data, r->p[X], r->p[Y]);
	    }
	}

	return 0;
    }
