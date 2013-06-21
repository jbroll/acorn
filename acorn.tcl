#!/Users/john/bin/tclkit8.6
#
if { 0 && ![critcl::compiled] } {		# Force md5 to be FAST.  Don't use object cache.
    proc md5 { x } {
	    return [expr rand()]
    }
}

::critcl::tcl 8.6
::critcl::config language c++ 

critcl::cheaders -I/Users/john/include -I/home/john/include

critcl::tsources util/func.tcl util/tcloo.tcl util/unix.tcl 		\
		 util/rays.tcl						\
		 zmx-io/uda.tcl zmx-io/agf.tcl				\
		 acorn-model.tcl acorn.tcl				\
		 zmx-io/zmx.tcl

critcl::csources acorn.cpp aperture.cpp glass/acorn-glass.cpp glass/glass.c tpool/tpool.c

critcl::clibraries -lstdc++


source util/tcloo.tcl

namespace eval acorn {
    variable SurfaceTypes
    variable SurfaceInfos


    critcl::ccode {
	#include <dlfcn.h>


	extern "C" {
	    int  SurfSize(void);
	    int  RaysSize(void);

	    void prays(void *r, int n);

	    void trace_rays(double z, double n, void *surflist, int nsurfs, void *ray, int nray, int rsize, int nthread, char *xray);

	    typedef int  (*InfosFunc)(int info, char ***str, double **val);
	    double glass_indx(void *glass, double wave);
	    int  GlasSize(void);
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

	set ::acorn::SurfaceTypes(coordbrk)	-1
	set ::acorn::SurfaceInfos(coordbrk)	-1

	foreach type [glob ./surfaces/lib/[arch]/*.so] {
	    if { ![set ::acorn::SurfaceTypes([file rootname [file tail $type]]) [acorn::getsymbol $type traverse]] } {
		error "Cannot load traverse from $type"
	    }
	    if { ![set ::acorn::SurfaceInfos([file rootname [file tail $type]]) [acorn::getsymbol $type info]] } {
		error "Cannot load traverse from $type"
	    }
	}

	arec::typedef ::acorn::Rays {
	    double	px, py, pz, kx, ky, kz;
	    int		vignetted;
	}

	arec::typedef ::acorn::Surfs {
	    string	name
	    string	type
	    string	comment
	    long	traverse
	    long	infos

	    string	aper_type;
	    string	aper_param;

	    string	aperture;
	    long	aper_data;
	    long	aper_leng;

	    foreach i [iota 0 255] { double p$i }
	    foreach i [iota 0  31] { char*  s$i }

	    string	glass
	    long	glass_ptr

	    long	enable
	}
	arec::typedef ::acorn::SurfaceList {
	    long 	 surf;
	    int		nsurf;
	    int		 type;
	}

	if { [acorn::RaysSize] != [::acorn::Rays  size] } { error "acorn size mismatch Rays  [acorn::RaysSize] != [::acorn::Rays  size]" }
	if { [acorn::SurfSize] != [::acorn::Surfs size] } { error "acorn size mismatch Surfs [acorn::SurfSize] != [::acorn::Surfs size]" }
	if { [acorn::GlasSize] != [::acorn::Glass size] } { error "acorn size mismatch Glass [acorn::GlasSize] != [::acorn::Glass size]" }

	glass-loader glass
    }

    critcl::cproc RaysSize {} int { return RaysSize(); }
    critcl::cproc SurfSize {} int { return SurfSize(); }
    critcl::cproc GlasSize {} int { return GlasSize(); }

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
    critcl::cproc trace_rays { double z double n long s int nsurf long r int nray int rsize int nthread long xray } void {
                  trace_rays(z, n, (void *) s, nsurf, (void *) r, nray, rsize, nthread, (char *) xray); 
    }
    critcl::cproc glass_indx { long glass double wave } double {
                  return glass_indx((void*)glass, wave); 
    }

    critcl::cproc _prays { long r int nray } void { prays((void *) r, nray); }

    if { ![::critcl::compiled] } {
	critcl::cproc arch   {} {const char*} [subst { return "$env(ARCH)"; }]
    }
}


proc ::acorn::Aperture { type param } {
    #set type $type

    switch $type {
	circular -
	annulus  -
	(null)   -
	{}	{ return {} } 
	UDA 	{ return [[UDA create uda[incr ::UDA] source $param] polygon] }
	default { error "Unknown aperture type : $type" }
    }
}

package provide acorn 1.0

if { [::critcl::compiled] } {
    acorn::init
}
