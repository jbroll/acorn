#!/Users/john/bin/tclkit8.6
#

critcl::cheaders -I/Users/john/include
critcl::clibraries acorn.o 
critcl::tsources acorn.tcl

::critcl::tcl 8.6

#critcl::config language c++
critcl::clibraries -lstdc++


namespace eval acorn {
    variable SurfaceTypes

    critcl::ccode {
	#include <dlfcn.h>

	void trace(double z, double n, void *surfs, int nsurf, void *rays, int nray);
    }

    proc init {} {
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

	arec::typedef RayType {
	    double	px py pz
	    double	kx ky kz
	    int		vignetted;
	}

	arec::typedef SurfType {
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

	if { [acorn::SurfSize] != [SurfType size] } { error "acorn size mismatch [acorn::SurfSize] != [SurfType size]" }
    }

    critcl::cproc SurfSize {} int { return SurfSize(); }

    critcl::cproc getsymbol { Tcl_Interp* interp char* libso char* symbol } long {
	void *handle = dlopen(libso, RTLD_NOW);
	void *addres = dlsym(handle, symbol);

		//printf("%p\n", addres);

	return (long) addres;
    }

    critcl::cproc trace_seq { double z double n long s int nsurf long r int nray } void { trace_surfaces_seq(z, n, s, nsurf, r, nray); }
    critcl::cproc trace_nsq { double z double n long s int nsurf long r int nray } void { trace_surfaces_nsq(z, n, s, nsurf, r, nray); }
}

package provide acorn 1.0
