#!/Users/john/bin/tclkit8.6
#
::critcl::tcl 8.6
::critcl::config language c++ 

critcl::cflags -O3
critcl::cheaders -I/Users/john/include -I/home/john/include
critcl::cheaders rays.h arec/arec.h

critcl::tsources rays.tcl
critcl::clibraries -lstdc++ -Lranlib -lranlib

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

	using namespace std;
	using namespace Eigen;

	#include "rays.h"
	#include "arec.h"

	extern "C" {
	    float gennor(float av,float sd);
	    long ignpoi(float mu);
	}


	    void swap2(char *to, char *from, long nbytes)
	    {
		char c;
		long i;
		for ( i=0; i < nbytes; i += 2, (to += 2), (from += 2) ) {
		    c = *from;
		    *(to) = *(from+1);
		    *(to+1) = c;
		}
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

    critcl::cproc ::acorn::Rays::angles { Tcl_Interp* ip double ax double ay } ok {
	ARecPath *path = (ARecPath *) clientdata;
	Ray      *rays = (Ray *)path->recs;

	for ( int i = path->first; i <= path->last; i++ ) {
	    rays[i].k[X] = sin(ax/57.2957795);
	    rays[i].k[Y] = sin(ay/57.2957795);
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



    critcl::cproc ::acorn::Rays::bin { Tcl_Interp* ip char* Type int nx int ny double sx double sy int radius double sigma double bias double noise int poison } ok {
#define TY_USHORT	1
	ARecPath *path = (ARecPath *) clientdata;

	int	 type;
	int      size;
	void    *data;

        float    cx, cy, h;
        int      x, y, r = radius;

	Ray *rays = (Ray *)path->recs;

	if ( !strcmp(Type, "ushort" ) ) {
	    type = TY_USHORT;
	    size = sizeof(unsigned short);
	}

	data = malloc(nx * ny * size);

	double sum = 0;



        sum = 0;

        for ( y = -r/2; y < r/2; y++ ) {
            for ( x = -r/2; x < r/2; x++ ) {
                    double squ = x*x + y*y;

                     sum += exp(-squ/sigma);
            }
        }

        h /= sum;       // Normalize the height for the volume of the gausian.


	for ( y = 0; y < ny; y++ ) {
	    for ( x = 0; x < nx; x++ ) {
		switch ( type ) {
		 case TY_USHORT : ((unsigned short *) data)[y*nx + x] = gennor(bias, noise); break;
		}
	    }
	}

	for ( int i = path->first; i <= path->last; i++ ) {

	    cx = rays[i].p[X]/sx;
	    cy = rays[i].p[Y]/sy;
	     h = rays[i].intensity;

	    cx += nx/2;
	    cy += ny/2;

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
	}

	for ( y = 0; y < ny; y++ ) {
	    for ( x = 0; x < nx; x++ ) {
		
		switch ( type ) {
		 case TY_USHORT :
		     if ( poison ) { ((unsigned short *) data)[y*nx + x] = ignpoi(((unsigned short *) data)[y*nx + x]); }
		     ((unsigned short *) data)[y*nx + x] -= 32768;     break;
		}
	    }
	}

	swap2((char*) data, (char *)data, nx*ny*size);

	Tcl_SetObjResult(ip, Tcl_NewByteArrayObj((const unsigned char *) data, nx*ny*size));

	return TCL_OK;
	
    } -pass-cdata true

    critcl::cproc ::acorn::Rays::stat { Tcl_Interp* ip int { v 0 } } ok {
	ARecPath *path = (ARecPath *) clientdata;

	printf("Here in stat\n");

	Ray *rays = (Ray *)path->recs;

	int     n = 0;

	double x  = 0;
	double y  = 0;
	double r  = 0;
	double cx = 0;
	double cy = 0;

	double rx, ry, rr;

	for ( int i = path->first; i <= path->last; i++ ) {
	    if ( rays[i].p[X] != rays[i].p[X]
	      || rays[i].p[Y] != rays[i].p[Y] 
	      || rays[i].p[Z] != rays[i].p[Z] ) { continue; }
	    if ( v && rays[i].vignetted   )     { continue; }

	    cx += rays[i].p[X];
	    cy += rays[i].p[Y];

	    rr = sqrt((rays[i].p[X]-cx) * (rays[i].p[X]-cx) + (rays[i].p[Y]-cy) * (rays[i].p[Y]-cy));

	    x += (rays[i].p[X]-cx) * (rays[i].p[X]-cx);
	    y += (rays[i].p[Y]-cy) * (rays[i].p[Y]-cy);
	    r +=  rr*r;

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

	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(cx));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(cy));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(rx));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(ry));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewDoubleObj(rr));
	Tcl_ListObjAppendElement(ip, result, Tcl_NewIntObj( n));

	Tcl_SetObjResult(ip, result);

	return TCL_OK;
    } -pass-cdata true


