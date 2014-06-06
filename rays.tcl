#!/Users/john/bin/tclkit8.6
#
::critcl::tcl 8.6
::critcl::config language c++ 

critcl::cflags -O3
critcl::cheaders -I/Users/john/include -I/home/john/include
critcl::cheaders rays.h arec/arec.h

critcl::tsources rays.tcl
critcl::clibraries -lstdc++

package provide rays 0.1

if { [::critcl::compiled] } {
    package require arec

    arec::typedef ::acorn::Rays {
	double	px, py, pz, kx, ky, kz;
	int	vignetted;
    }

    proc ::acorn::mkrays { name args } {
	if { $name eq "-" } { set name rays[incr ::acorn::RAYS] }
	set pz 0
	set args [dict merge { type acorn::Rays circle 0 nx 11 ny 11 x0 -5 x1 5 y0 -5 y1 5 xi - yi - } $args]


	dict with args {
	    if { [info commands $name] eq {} } { $type create $name 0 }

	    if { [info exists box] } {
		set x0 [expr -$box]
		set x1 [expr  $box]
		set y0 [expr -$box]
		set y1 [expr  $box]
	    }

	    set i 0
	    foreach x [jot $nx $x0 $x1 $xi] {
		foreach y [jot $ny $y0 $y1 $yi] {
		    if { $circle && $x*$x+$y*$y > $x0*$x0+$y0+$y0 } { continue }

		    $name set end+1 px $x py $y pz $pz kx 0.0 ky 0.0 kz 1.0 vignetted 0
		    incr i
		}
	    }
	}

	return $name
    }

    proc ::acorn::prays { rays { pipe {} } { out stdout } } {

	if { $pipe eq ">" } { set out [open $out w] }

	set i 1

	puts $out "id	x	y	z	l	m	n	v"
	puts $out "-	-	-	-	--	--	--	-"

	foreach row [$rays get px py pz kx ky kz vignetted] {
	    puts $out "$i	[join $row \t]"
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
    }

    critcl::cproc ::acorn::Rays::angles { Tcl_Interp* ip double ax double ay } ok {
	ARecPath *path = (ARecPath *) clientdata;

	Ray *rays = (Ray *)path->recs;
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


