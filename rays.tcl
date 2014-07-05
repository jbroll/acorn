#!/Users/john/bin/tclkit8.6
#
::critcl::tcl 8.6
::critcl::config language c++ 

critcl::cflags -O3
critcl::cheaders -I/Users/john/include -I/home/john/include
critcl::cheaders rays.h arec/arec.h xtypes.h

critcl::tsources rays.tcl
critcl::clibraries -lstdc++ -Lranlib -lranlib

if { ![::critcl::compiled] } {
    source doubleList.tcl
}

package provide rays 0.1

if { [::critcl::compiled] } {
    package require arec

    arec::typedef ::acorn::Rays {
	double	px, py, pz, kx, ky, kz;
	int	wave;
	int	vignetted;
	float	intensity;
    }

    proc ::acorn::mkrays { name args } {
	set args [dict merge { type acorn::Rays circle 0 nx 11 ny 11 x0 -5 x1 5 y0 -5 y1 5 xi - yi - intensity 1 } $args]

	dict with args {}

	if { $name eq "-" } 		   { set name [$type new [expr { $nx*$ny }]] }
	if { [info commands $name] eq {} } { $type create $name  [expr { $nx*$ny }]  }


	if { [info exists diameter] } {
	    set radius [expr $diameter/2.0]
	}

	if { [info exists radius] } {
	    set circle 1
	    set box $radius
	}

	if { [info exists box] } {
	    set x0 [expr -$box]
	    set x1 [expr  $box]
	    set y0 [expr -$box]
	    set y1 [expr  $box]
	}
	if { $xi eq "-" } { set xi [expr { ($x1-$x0)/($nx-1.0) }] }
	if { $yi eq "-" } { set yi [expr { ($y1-$y0)/($ny-1.0) }] }

	$name length = [$name mkrays : $nx $x0 $x1 $xi $ny $y0 $y1 $yi $intensity $circle]

	return $name
    }

    proc ::acorn::prays { rays { pipe {} } { out stdout } } {
	set format {}

	switch $pipe {
	  ">" { set out [open $out w] }
	  "|" { set out [open |$out w] }
	  {} {}
	  default { set format $pipe }
	}

	set i 1

	puts $out "id	x	y	z	l	m	n	v	i	w"
	puts $out "-	-	-	-	--	--	--	-	-	-"

	foreach row [$rays get px py pz kx ky kz vignetted intensity wave] {
	    if { $format eq {} } {
		set line $row
	    } else {
		set line [lmap x $row { format $format $x }]
	    }

	    puts $out "$i	[join $line \t]"
	    incr i
	}

	if { $pipe eq ">" } { close $out }
    }

    proc ::acorn::rays-foreach { rays body } {
	set cmd [subst { foreach _i \[iota 0 [$rays length]-1] {
	    lassign \[$rays get \$_i] [$rays type names]
	    $body
	}}]

	uplevel 1 $cmd
    }

    proc ::acorn::rays-select { rays expr { cols {} } } {
	if { $cols eq "" } {
	    set cols [$rays type names]
	}

	uplevel [subst {
	    set _1 {}
	    rays-foreach $rays { if { !($expr) } { continue } ; lappend _1 $[join $cols " $"] }
	    return \$_1
	}]
    }
}

    critcl::ccode {
	#include <Eigen/Dense>

	#include <float.h>

	using namespace std;
	using namespace Eigen;

	#include "rays.h"
	#include "arec.h"
	#include "xtypes.h"

	int xrand() {
	    int x;

	    while ( (x = rand()) < 100 ) {};

	    return x;
	}

	extern "C" {
	    float gennor(float av,float sd);
	}



    }

    critcl::cproc ::acorn::Rays::mkrays { Tcl_Interp* ip double nx double x0 double x1 double xi
							 double ny double y0 double y1 double yi
						double intensity int circle } int {
	ARecPath *path = (ARecPath *) clientdata;
	Ray      *rays = (Ray *)path->recs;
	int          i = path->first;

	double x, y;
	int   xth, yth;


	for ( xth = nx, x = x0; xth--; x += xi ) {
	for ( yth = nx, y = y0; yth--; y += yi ) {
	    if ( circle && x*x+y*y > x0*x0+y0+y0 ) { continue; }

	    rays[i].p[X] =   x;
	    rays[i].p[Y] =   y;
	    rays[i].p[Z] = 0.0;
	    rays[i].k[X] = 0.0;
	    rays[i].k[Y] = 0.0;
	    rays[i].k[Z] = 1.0;

	    rays[i].intensity = intensity;
	    rays[i].vignetted = 0;
	    rays[i].wave      = 0;

	    if ( ++i > path->last ) { break; }
	}
	}

	return i;
    } -pass-cdata true


    critcl::cproc ::acorn::Rays::waves { Tcl_Interp* ip doubleList waves } ok {
	ARecPath *path = (ARecPath *) clientdata;
	Ray      *rays = (Ray *)path->recs;

	int       nray = path->last-path->first+1;

	double cumb = 0.0;
	double N = nray/waves.length;

	int i;
	int j = 0;
	for ( i = 0; i < waves.length; i++ ) {					// Assign wave number according to cumbulative probability
	    cumb += waves.list[i];
	    int max = cumb*N > nray ? nray : (cumb*waves.length*N+0.5);

	    for ( ; j < nray && j < max; j++ ) {
		rays[j].wave = i;
	    }
	}

	while ( j < nray ) { rays[j++].wave = i-1; }				// Maye one or two left over?

	for ( int i = path->last; i >= path->first+1; i-- ) {			// Shuffle
	    int j = xrand() % i;

	    int tmp = rays[i].wave; rays[i].wave = rays[j].wave; rays[j].wave = tmp;
	}

	return TCL_OK;
    } -pass-cdata true

    critcl::cproc ::acorn::Rays::angles { Tcl_Interp* ip double ax double ay double { fwhm 0.0 } } ok {
	ARecPath *path = (ARecPath *) clientdata;
	Ray      *rays = (Ray *)path->recs;

	fwhm /= 3600;

	double aax = ax;
	double aay = ay;

	for ( int i = path->first; i <= path->last; i++ ) {
	    if ( fwhm ) {
		aax = ax + gennor(0.0, fwhm/2.35);
		aay = ay + gennor(0.0, fwhm/2.35);
	    }

	    rays[i].k[X] = sin(aax/57.2957795);
	    rays[i].k[Y] = sin(aay/57.2957795);
	    rays[i].k[Z] = 1.0;


	    rays[i].k.normalize();
	    
	}

	return TCL_OK;
    } -pass-cdata true

    critcl::cproc ::acorn::Rays::advance { Tcl_Interp* ip double dist } ok {
	ARecPath *path = (ARecPath *) clientdata;

	Ray *rays = (Ray *)path->recs;
	for ( int i = path->first; i <= path->last; i++ ) {
	    rays[i].p += dist * rays[i].k;					// Move along the ray the distance requested.
	}

	return TCL_OK;
    } -pass-cdata true




    critcl::cproc ::acorn::Rays::bin { Tcl_Interp* ip char* Type Tcl_Obj* Data int nx int ny double sx double sy int { radius 1 } double { sigma 0 } } ok {
	ARecPath *path = (ARecPath *) clientdata;

	int	 type;
	int      size, length;

	void    *data = Tcl_GetByteArrayFromObj(Data, &length);

        float    cx, cy, h;
        int      x, y, r = radius;

	Ray *rays = (Ray *)path->recs;

	if ( !strcmp(Type, "ushort" ) ) {
	    type = TY_USHORT;
	    size = sizeof(unsigned short);
	}

	double sum = 0;

	if ( radius > 1 ) {
	    for ( y = -r/2; y < r/2; y++ ) {
		for ( x = -r/2; x < r/2; x++ ) {
			double squ = x*x + y*y;

			 sum += exp(-squ/sigma);
		}
	    }

	    h /= sum;       // Normalize the height for the volume of the gausian.
	}

	for ( int i = path->first; i <= path->last; i++ ) {

	    if ( rays[i].vignetted ) { continue; }

	    cx = rays[i].p[X]/sx;
	    cy = rays[i].p[Y]/sy;
	     h = rays[i].intensity;

	    cx += nx/2;
	    cy += ny/2;

	    if ( radius > 1 ) {
		for ( y = -r/2; y < r/2; y++ ) {
		    for ( x = -r/2; x < r/2; x++ ) {
			if ( cx+r < 0 || cx-r >= nx ) { continue; }
			if ( cy+r < 0 || cy-r >= ny ) { continue; }

			double fx = cx - (int) cx;
			double fy = cy - (int) cy;

			int xi = (int)cx + x;
			int yi = (int)cy + y;

			if ( xi < 0 || xi >= nx ) { continue; }
			if ( yi < 0 || yi >= ny ) { continue; }

			double squ = (x+fx)*(x+fx) + (y+fy)*(y+fy);

			switch ( type ) {
			 case TY_USHORT : ((unsigned short *) data)[yi*nx + xi] += h*exp(-squ/sigma); break;
			}
		    }
		}
	    } else {
		int ix = cx + 0.5;
		int iy = cy + 0.5;

		if ( ix >= 0 && ix < nx && ix >= 0 && ix < ny ) {
		    switch ( type ) {
		     case TY_USHORT : ((unsigned short *) data)[ix*nx + iy] += h; break;
		    }
		}
	    }
	}

	return TCL_OK;
    } -pass-cdata true

    critcl::cproc ::acorn::Rays::stat { Tcl_Interp* ip int { v 0 } } ok {
	ARecPath *path = (ARecPath *) clientdata;

	Ray *rays = (Ray *)path->recs;

	int     n = 0;

	double x  = 0;
	double y  = 0;
	double r  = 0;
	double cx = 0;
	double cy = 0;

	double xmin =  DBL_MAX;
	double xmax = -DBL_MAX;
	double ymin =  DBL_MAX;
	double ymax = -DBL_MAX;

	double rx, ry, rr;

	for ( int i = path->first; i <= path->last; i++ ) {
	    if ( rays[i].p[X] != rays[i].p[X]
	      || rays[i].p[Y] != rays[i].p[Y] 
	      || rays[i].p[Z] != rays[i].p[Z] ) { continue; }
	    if ( !v && rays[i].vignetted   )     { continue; }

	    cx += rays[i].p[X];
	    cy += rays[i].p[Y];

	    rr = sqrt((rays[i].p[X]-cx) * (rays[i].p[X]-cx) + (rays[i].p[Y]-cy) * (rays[i].p[Y]-cy));

	    x += (rays[i].p[X]-cx) * (rays[i].p[X]-cx);
	    y += (rays[i].p[Y]-cy) * (rays[i].p[Y]-cy);
	    r +=  rr*r;

	    if ( rays[i].p[X] < xmin ) { xmin = rays[i].p[X]; }
	    if ( rays[i].p[X] > xmax ) { xmax = rays[i].p[X]; }
	    if ( rays[i].p[Y] < ymin ) { ymin = rays[i].p[Y]; }
	    if ( rays[i].p[Y] > ymax ) { ymax = rays[i].p[Y]; }

	    n++;
	}

	if ( n ) {
	    cx /= n;
	    cy /= n;

	    rx = x != 0.0 ? sqrt(x/n) : 0.0;
	    ry = x != 0.0 ? sqrt(x/n) : 0.0;
	    rr = r != 0.0 ? sqrt(r/n) : 0.0;
	} else {
	    cx = 0;
	    cy = 0;
	    rx = 0;
	    ry = 0;
	    rr = 0;
	}

	Tcl_Obj *result = Tcl_NewObj();

	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("x", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj((xmax+xmin)/2.0));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("y", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj((ymax+ymin)/2.0));

	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("xmin", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(xmin));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("xmax", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(xmax));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("ymin", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(ymin));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("ymax", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(ymax));

	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("xrange", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(xmax-xmin));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("yrange", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(ymax-ymin));

	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("cx", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(cx));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("cy", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(cy));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("rx", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(rx));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("ry", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(ry));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("rr", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(rr));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewStringObj("n", -1));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewIntObj( n));

	Tcl_SetObjResult(ip, result);

	return TCL_OK;
    } -pass-cdata true


