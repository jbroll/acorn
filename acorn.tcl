#!/Users/john/bin/tclkit8.6
#

critcl::cheaders -I/Users/john/include -I/home/john/include

critcl::tsources func.tcl tcloo.tcl acorn.tcl uda.tcl
critcl::csources acorn.cpp aperture.cpp

::critcl::tcl 8.6
::critcl::config language c++ 

critcl::clibraries -lstdc++


source tcloo.tcl
#source uda.tcl

namespace eval acorn {
    variable SurfaceTypes
    variable SurfaceInfos

    critcl::ccode {
	#include <dlfcn.h>

	extern "C" {
	    int  SurfSize(void);
	    int  RaysSize(void);

	    void xrays(void *r, int n);
	    void prays(void *r, int n);
	    void trace_rays(double z, double n, void *surflist, int nsurfs, void *ray, int nray);
	    typedef int  (*InfosFunc)(int info, char ***str, double **val);
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

	set ::acorn::SurfaceTypes(coordbk)	-1

	foreach type [glob ./surfaces/*.so] {
	    set ::acorn::SurfaceTypes([file rootname [file tail $type]]) [acorn::getsymbol $type traverse]
	    set ::acorn::SurfaceInfos([file rootname [file tail $type]]) [acorn::getsymbol $type info]
	}

	arec::typedef ::acorn::Rays {
	    double	px, py, pz, kx, ky, kz;
	    int		vignetted;
	}

	arec::typedef ::acorn::Surfs {
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

	    string	aper_type;
	    double	aper_min;
	    double	aper_max;
	    string	aper_param;

	    string	aperture;
	    long	aper_data;
	    long	aper_leng;

	    foreach i [iota 0 255] { double p$i }
	    foreach i [iota 0  31] { char*  s$i }

	    string	name
	    string	type
	    long	traverse
	    long	infos

	    string	glass
	}
	arec::typedef ::acorn::SurfaceList {
	    long 	 surf;
	    int		nsurf;
	    int		 type;
	}

	if { [acorn::RaysSize] != [::acorn::Rays  size] } { error "acorn size mismatch [acorn::RaysSize] != [::acorn::Rays  size]" }
	if { [acorn::SurfSize] != [::acorn::Surfs size] } { error "acorn size mismatch [acorn::SurfSize] != [::acorn::Surfs size]" }
    }

    critcl::cproc SurfSize {} int { return SurfSize(); }
    critcl::cproc RaysSize {} int { return RaysSize(); }

    critcl::cproc getsymbol { Tcl_Interp* interp char* libso char* symbol } long {
	void *handle = dlopen(libso, RTLD_NOW);
	void *addres = dlsym(handle, symbol);

	return (long) addres;
    }

    critcl::cproc infos { Tcl_Interp* ip int info long infos } ok {
    	char   **str;
	double *val;

	int n = ((InfosFunc) infos)(info, &str, &val);

	Tcl_Obj *result = Tcl_GetObjResult(ip);
	Tcl_Obj  	 *strs = Tcl_NewObj();
	Tcl_Obj  	 *vals = Tcl_NewObj();
	int i;

	for ( i = 0; i < n; i++ ) {
	    Tcl_ListObjAppendElement(ip, strs , Tcl_NewStringObj(str[i], -1));
	    Tcl_ListObjAppendElement(ip, vals , Tcl_NewDoubleObj(val[i]));
	}	

	Tcl_ListObjAppendElement(ip, result, strs);
	Tcl_ListObjAppendElement(ip, result, vals);

	return TCL_OK;
    }
    critcl::cproc trace_rays { double z double n long s int nsurf long r int nray } void {
                  trace_rays(z, n, (void *) s, nsurf, (void *) r, nray); 
    }

    critcl::cproc _prays { long r int nray } void { prays((void *) r, nray); }
    critcl::cproc _xrays { long r int nray } void { xrays((void *) r, nray); }
}


proc ::acorn::Aperture { type param } {
    set type [lindex [lindex $type 0] 0]

    switch $type {
	circular -
	annulus  -
	{}	{ return {} } 
	UDA 	{ return [[UDA create uda[incr ::UDA] source $param] polygon] }
	default { error "Unknown aperture type : $type" }
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
