#!/Users/john/bin/tclkit8.6
#
::critcl::tcl 8.6
::critcl::config language c++ 

critcl::cflags -O3
critcl::cheaders -I/Users/john/include -I/home/john/include -I/home/jroll/include
critcl::cheaders arec/arec.h acorn.h rays.h xtypes.h

critcl::tsources jbr.tcl/func.tcl jbr.tcl/tcloo.tcl			\
		 jbr.tcl/fits.tcl jbr.tcl/unix.tcl			\
		 zmx-io/uda.tcl zmx-io/agf.tcl				\
		 acorn-model.tcl acorn.tcl				\
		 zmx-io/zmx.tcl						\
		 tcltest.tcl

critcl::csources acorn.cpp aperture.cpp glass/acorn-glass.cpp glass/glass.c tpool/tpool.c

critcl::clibraries -lstdc++ -Lranlib -lranlib


try { set ::ACORN    $env(ACORN)/ 		} on error message { 	set ::ACORN 	.			}
try { set ::Surfaces $env(ACORN_SURFACES)	} on error message { 	set ::Surfaces ${::ACORN}/surfaces 	}
try { set ::GlassDir $env(ACORN_GLASS)		} on error message { 	set ::GlassDir ${::ACORN}/glass		}

if { ![::critcl::compiled] } {
    source doubleList.tcl
}

namespace eval acorn {
    variable SurfaceTypes
    variable SurfaceInfos
    variable SurfaceInits

    critcl::ccode {
	#include <dlfcn.h>
	#include <Eigen/Dense>

	using namespace std;
	using namespace Eigen;

	#include "arec.h"
	#include "acorn.h"
	#include "xtypes.h"

	extern "C" {
	    void setall(long seed0, long seed1);

	    float gennor(float av,float sd);
	    long ignpoi(float mu);


	    int  SurfSize(void);
	    int  RaysSize(void);

	    void prays(void *r, int n);

	    void trace_rays(void *mdata, void *surflist, int nsurfs, void *ray, int nray, int rsize, int nthread, char *xray);

	    //typedef int  (*InfosFunc)(int info, char ***str, double **val);
	    double glass_indx(void *glass, double wave);
	    int  GlasSize(void);
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

    critcl::cinit {
	setall((long) &setall % 2147483398 + 1, time(NULL) % 2147483398 + 1);
    } {}

    proc init {} {
	package require arec

	proc ::arec::Affine3d { name } {
	    foreach axis { x y z w } {
		foreach col { 1 2 3 4 } {
		    double $name-$axis$col
		}
	    }
	}

	foreach type [glob $::Surfaces/lib/[arch]/*.so] {
	    if { ![set ::acorn::SurfaceTypes([file rootname [file tail $type]]) [acorn::getsymbol $type traverse]] } {
		error "Cannot load traverse from $type"
	    }
	    if { ![set ::acorn::SurfaceInfos([file rootname [file tail $type]]) [acorn::getsymbol $type info]] } {
		error "Cannot load info from $type"
	    }
	    if { ![set ::acorn::SurfaceInits([file rootname [file tail $type]]) [acorn::getsymbol $type inits]] } {
		set ::acorn::SurfaceInits([file rootname [file tail $type]]) 0
	    }
	}

	arec::typedef ::acorn::ModelData {
	    double	z
	    long        indicies, wavelengths;
	    int		nwave;
	}

	arec::typedef ::acorn::Surfs {
	    string	name
	    string	type
	    string	comment
	    long	traverse
	    long	infos
	    long	inits

	    string	aper_type;
	    string	aper_param;

	    string	aperture;
	    long	aper_data;
	    long	aper_leng;

	    foreach i [iota 0 255] { double p$i }
	    foreach i [iota 0  31] { char*  s$i }

	    string	glass
	    long	glass_ptr
	    long	indicies

	    long	enable
	    long	annot
	    long	data
	}
	arec::typedef ::acorn::SurfaceList {
	    long 	 surf;
	    int		nsurf;
	    int		 type;
	}

	if { [acorn::RaysSize] != [::acorn::Rays  size] } { error "acorn size mismatch Rays  [acorn::RaysSize] != [::acorn::Rays  size]" }
	if { [acorn::SurfSize] != [::acorn::Surfs size] } { error "acorn size mismatch Surfs [acorn::SurfSize] != [::acorn::Surfs size]" }
	if { [acorn::GlasSize] != [::acorn::Glass size] } { error "acorn size mismatch Glass [acorn::GlasSize] != [::acorn::Glass size]" }

	glass-loader $::GlassDir
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

	Tcl_Obj *result = Tcl_GetObjResult(ip);
	Tcl_Obj  	 *strs = Tcl_NewObj();
	Tcl_Obj  	 *vals = Tcl_NewObj();
	int i;

	if ( info == 1 ) {
	    char   **str;
	    double *val;

	    int n = ((InfosFunc) infos)(info, &str, &val);


	    for ( i = 0; i < n; i++ ) {
		Tcl_ListObjAppendElement(ip, strs , Tcl_NewStringObj(str[i], -1));
		Tcl_ListObjAppendElement(ip, vals , Tcl_NewDoubleObj(val[i]));
	    }	
	}
	if ( info == 2 ) {
	    char   **str;
	    char   **val;

	    int n = ((InfosFunc) infos)(info, &str, (double **)&val);

	    for ( i = 0; i < n; i++ ) {
		Tcl_ListObjAppendElement(ip, strs , Tcl_NewStringObj(str[i], -1));
		Tcl_ListObjAppendElement(ip, vals , Tcl_NewStringObj(val[i], -1));
	    }	
	}


	Tcl_ListObjAppendElement(ip, result, strs);
	Tcl_ListObjAppendElement(ip, result, vals);

	return TCL_OK;
    }

    critcl::cproc inits { Tcl_Interp* ip long inits long model long surface long ray } ok {
	    ((TraceFunc) inits)((MData *) model, (Surface *) surface, (Ray *) ray);

	    return TCL_OK;
    }

    critcl::cproc trace_rays { long m long s int nsurf long r int nray int rsize int nthread long xray } void {
                  trace_rays((void *) m, (void *) s, nsurf, (void *) r, nray, rsize, nthread, (char *) xray); 
    }
    critcl::cproc glass_indx { long glass double wave } double {
                  return glass_indx((void*)glass, wave); 
    }
    critcl::cproc glass_indicies { long glass doubleList waves } long {
	int i;

	for ( i = 0; i < waves.length; i++ ) {
	    waves.list[i] = glass_indx((void*)glass, waves.list[i]); 
	}

	return (long) waves.list;
    }

    critcl::cproc doubleList { doubleList doubles } long {
	return (long) doubles.list;
    }

    critcl::cproc malloc { long size } Tcl_Obj* {
	Tcl_Obj* obj = Tcl_NewByteArrayObj(NULL, size);
	Tcl_IncrRefCount(obj);

	unsigned char *bytes = Tcl_GetByteArrayFromObj(obj, NULL);
	memset(bytes, 0, size);

	return obj;
    }
    critcl::cproc free   { long pointer } void { free((void *) pointer);	}

    critcl::cproc ::acorn::noise   { Tcl_Interp* ip char* Type Tcl_Obj* Data int nx int ny double bias double noise } ok {
	int	 type, x, y, length;

	unsigned char *data = Tcl_GetByteArrayFromObj(Data, &length);

	if ( !strcmp(Type, "ushort" ) ) {
	    type = TY_USHORT;
	}
	for ( y = 0; y < ny; y++ ) {
	    for ( x = 0; x < nx; x++ ) {
		switch ( type ) {
		 case TY_USHORT : ((unsigned short *) data)[y*nx + x] += (short unsigned int) gennor(bias, noise); break;
		}
	    }
	}

	return TCL_OK;
    }

    critcl::cproc ::acorn::poisson { Tcl_Interp* ip char* Type Tcl_Obj* Data int nx int ny } ok {
	int	 type, x, y, length;

	unsigned char *data = Tcl_GetByteArrayFromObj(Data, &length);

	if ( !strcmp(Type, "ushort" ) ) {
	    type = TY_USHORT;
	}

	for ( y = 0; y < ny; y++ ) {
	    for ( x = 0; x < nx; x++ ) {
		switch ( type ) {
		 case TY_USHORT : {
		    int value  = (int) (((unsigned short *) data)[y*nx + x]);

		    if ( value ) {
			int noise = ignpoi(value);

			if ( value + noise > 0 ) {
			    ((unsigned short *) data)[y*nx + x] = value + noise;
			}
		    }
		    
		    break;
		  }
		}
	    }
	}

	return TCL_OK;
    }
    critcl::cproc ::acorn::fits { Tcl_Interp* ip char* Type Tcl_Obj* Data int nx int ny } ok {
	int	 type, size, x, y, length;

	unsigned char *data = Tcl_GetByteArrayFromObj(Data, &length);

	if ( !strcmp(Type, "ushort" ) ) {
	    type = TY_USHORT;
	    size = sizeof(unsigned short);
	}

	if ( type == TY_USHORT ) {
	    for ( y = 0; y < ny; y++ ) {
		for ( x = 0; x < nx; x++ ) {
		     ((unsigned short *) data)[y*nx + x] -= 32768; 
		}
	    }
	}
	swap2((char*) data, (char *)data, nx*ny*size);

	return TCL_OK;
    }

    critcl::cproc _prays { long r int nray } void { prays((void *) r, nray); }

    if { ![::critcl::compiled] } {
	critcl::cproc arch   {} {const char*} [subst { return "$env(ARCH)"; }]
    }
}


proc ::acorn::Aperture { type param } {
    #set type $type

    switch $type {
	obstruction -
	circular -
	annulus  -
	(null)   -
	{}	{ return {} } 
	UDA 	{ return [UDA create uda[incr ::UDA] source $param] }
	default { error "Unknown aperture type : $type" }
    }
}

package provide acorn 1.0

if { [::critcl::compiled] } {
    package require rays
    acorn::init
}
