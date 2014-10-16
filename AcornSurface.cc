
#include <glob.h>

static map<char*, void *(*)()> Surfaces = NULL;

struct AcornSurface *AcornLoadSurfaces() {
	std::string surfdir;

    if ( getenv("ACORN_SURFACES") ) {
	surfdir  = getenv("ACORN_SURFACES");
    } else {
	if ( getenv("ACORN") ) { surfdir = getenv("ACORN");
	} else { 		 surfdir = ".";
	}

	surfdir += "/surfaces";
    }

    surfdir += "/lib/";
    surfdir += getenv("ARCH");
    surfdir += "/*.so";

    glob_t filelist;

    int err = glob(surfdir.c_str(), 0, NULL, &globbuf);

    for (size_t i = 0; i < filelist.gl_pathc; i++) {
	printf("%s\n", globbuf.gl_pathv[i]);
    }
    globfree(&filelist);

    return NULL;
}


AcornSurface *AcornSurfaceConstructor(char *type) {
    if ( Surfaces == NULL ) {
	Surfaces = AcornLoadSurfaces();
    } 

    if ( Surfaces.find(type) != 1 ) {
	fprintf(stderr, "No surface of type : %s\n", type);
	return NULL;
    }

    return Surfaces[type]();
}
