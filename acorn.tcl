#!/Users/john/bin/tclkit8.6
#

critcl::cheaders -I/Users/john/include -I/home/john/include

#critcl::clibraries /usr/lib/x86_64-linux-gnu/libstdc++.so.6

critcl::tsources acorn.tcl tcloo.tcl

::critcl::tcl 8.6
critcl::clibraries acorn.o -lstdc++


source tcloo.tcl

namespace eval acorn {
    variable SurfaceTypes

    critcl::ccode {
	#include <dlfcn.h>
    }


    proc trace { rays args } {
	foreach group $args {
	    switch [$group type] {
		    sequential { acorn::trace_rays 0 1 [[$group s] getptr] [[$group s] length] [rays getptr] [rays length] 0 }
		non-sequential { acorn::trace_rays 0 1 [[$group s] getptr] [[$group s] length] [rays getptr] [rays length] 1 }
		default        { error "unknown surface type [$group type]" }
	    }
	}
    }

    proc init {} {
	package require arec

	proc ::arec::Affine3d { name } {
	    foreach axis { x y z w } {
		foreach col { 1 2 3 4 } {
		    double $name-$axis$col
		}
	    }
	}

	foreach type [glob ./surfaces/*.so] {
	    set ::acorn::SurfaceTypes([file rootname [file tail $type]]) [acorn::getsymbol $type traverse]
	}

	arec::typedef ::acorn::Rays {
	    double	px py pz kx ky kz
	    int		vignetted;
	}

	arec::typedef ::acorn::Surfs {
	    double	aper;
	    double	R;
	    double	K;
	    double	n;
	    double 	thickness;

	    double	x;
	    double	y;
	    double	z;

	    double	rx;
	    double	ry;
	    double	rz;

	    string	name
	    string	type
	    long	traverse
	}

	if { [acorn::RaysSize] != [::acorn::Rays  size] } { error "acorn size mismatch [acorn::RaysSize] != [::acorn::Rays  size]" }
	if { [acorn::SurfSize] != [::acorn::Surfs size] } { error "acorn size mismatch [acorn::SurfSize] != [::acorn::Surfs size]" }
    }

    critcl::cproc SurfSize {} int { return SurfSize(); }
    critcl::cproc RaysSize {} int { return RaysSize(); }

    critcl::cproc getsymbol { Tcl_Interp* interp char* libso char* symbol } long {
	void *handle = dlopen(libso, RTLD_NOW);
	void *addres = dlsym(handle, symbol);

		//printf("%p\n", addres);

	return (long) addres;
    }

    critcl::cproc trace_rays { double z double n long s int nsurf long r int nray int once } void { trace_surfaces(z, n, s, nsurf, r, nray, once); }

    critcl::cproc prays { long r int nray } void { prays(r, nray); }
    critcl::cproc xrays { long r int nray } void { xrays(r, nray); }
}

oo::class create acorn::surface {
    variable s type
    accessor s type

    constructor { args } {
	set type sequential
	set s [::acorn::Surfs new [namespace current]::surfs 1]

	$s set {*}[dict merge { type simple n 1.00 } $args [list name [string range [self] 2 end]]]

	$s set traverse $::acorn::SurfaceTypes([$s get type])
    }
}

oo::class create acorn::surface-group {
    variable s type
    accessor s type

    constructor { args } {
	set surfs [lindex $args end]
	set args  [lrange $args 0 end-1]
	set surfs [regsub -all -line -- {((^[ \t]*)|([ \t]+))#.*$} $surfs { }]	; # Remove comments

	foreach { name value } [dict merge { type sequential } $args] { set $name $value }

	set s [::acorn::Surfs new [namespace current]::surfs [expr [llength $surfs]/2]]

	set i 0
	foreach { name params } $surfs {
	    $s $i set {*}[dict merge { type simple n 1.00 } $params [list name $name]]

	    $s $i set traverse $::acorn::SurfaceTypes([$s $i get type])

	    incr i
	}

	#puts [$s 0 end get]
    }
}

proc map { args } {
    uplevel [subst {
	set _[info frame] {}
	foreach {*}[lrange $args 0 end-1] { lappend _[info frame] \[[lindex $args end]] }
	set _[info frame]
    }]
}

package provide acorn 1.0

if { [::critcl::compiled] } {
    acorn::init
}
