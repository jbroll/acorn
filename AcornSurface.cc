
#include <stdio.h>
#include <glob.h>
#include <dlfcn.h>

#include <map>
#include <string>

#include <cstdlib>

#include "Eigen/Dense"

using namespace Eigen;

#include "Acorn.hh"
#include "AcornRay.hh"
#include "AcornSurface.hh"

void SetVar(char *that, const char *name, int from_type, std::map<const char *, VarMap> *vtable, void *value) {

    if ( vtable->count(name) != 1 ) {
	fprintf(stderr, "acorn: no such parameter : %s\n", name);
	return;
    }

    VarMap v = vtable->at(name);

    switch ( v.type + from_type * 16 ) {
     case Type_int    + Type_double * 16 : *((int    *)((char*)that+v.offset)) = *((double *) value);			break;
     case Type_double + Type_double * 16 : *((double *)((char*)that+v.offset)) = *((double *) value);			break;
     case Type_string + Type_double * 16 : *((string *)((char*)that+v.offset)) = std::to_string(*((double *) value));	break;

     case Type_int    + Type_int    * 16 : *((int    *)((char*)that+v.offset)) = *((int *) value);			break;
     case Type_double + Type_int    * 16 : *((double *)((char*)that+v.offset)) = *((int *) value);			break;
     case Type_string + Type_int    * 16 : *((string *)((char*)that+v.offset)) = std::to_string(*((int *) value));	break;

     case Type_int    + Type_string * 16 : *((int    *)((char*)that+v.offset)) = std::stoi(*((string *) value));	break;
     case Type_double + Type_string * 16 : *((double *)((char*)that+v.offset)) = std::stod(*((string *) value));	break;
     case Type_string + Type_string * 16 : *((string *)((char*)that+v.offset)) = (*((string *) value));			break;
    }
}


static std::map<const char*, AcornSurface *(*)()> *Surfaces = NULL;

std::map<const char*, AcornSurface *(*)()> *AcornLoadSurfaces() {

    std::map<const char*, AcornSurface *(*)()> *constructors = new std::map<const char*, AcornSurface *(*)()>;
    std::string surfdir;

    if ( std::getenv("ACORN_SURFACES") ) {
	surfdir  = std::getenv("ACORN_SURFACES");
    } else {
	if ( std::getenv("ACORN") ) {	surfdir = std::getenv("ACORN");
	} else { 		 	surfdir = ".";
	}

	surfdir += "/surfaces";
    }

    surfdir += "/lib/";

    if ( std::getenv("ARCH") ) {
	surfdir += std::getenv("ARCH");
	surfdir += "/";
    }

    surfdir += "*.so";


    glob_t filelist;

    int err = glob(surfdir.c_str(), 0, NULL, &filelist);

    for (size_t i = 0; i < filelist.gl_pathc; i++) {


	void *lib 		= dlopen(filelist.gl_pathv[i], RTLD_NOW);
	if ( !lib ) {
	    fprintf(stderr, "Cannot load surface lib: %s : %s\n", filelist.gl_pathv[i], dlerror());
	    continue;
	}

	AcornSurface *(*fun)()  = (AcornSurface *(*)()) dlsym(lib, "AcornSurfConstructor");

	if ( !fun ) {
	    fprintf(stderr, "Cannot load surface sym: %s : %s\n", filelist.gl_pathv[i], dlerror());
	    continue;
	}


	char *here; 

	if ( (here = strrchr(filelist.gl_pathv[i], '/')) ) {
	    here++;
	} else {
	    here = filelist.gl_pathv[i];
	}

	*strrchr(here, '.') = '\0';
	
	(*constructors)[here] = NULL;
    }
    globfree(&filelist);

    return constructors;
}


AcornSurface *AcornSurfaceConstructor(const char *type) {
    if ( Surfaces == NULL ) {
	Surfaces = AcornLoadSurfaces();
    } 

fprintf(stderr, "AcornSurfaceConstructor %d\n", Surfaces->size());

    if ( Surfaces->count(type) != 1 ) {
	fprintf(stderr, "No surface of type : %s\n", type);
	return NULL;
    }

    return Surfaces->at(type)();
}
