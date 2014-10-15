#include <stdio.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <map>
#include <vector>

#include "../Acorn.hh"
#include "../AcornSurface.hh"
#include "../AcornSurfGrp.hh"
#include "../AcornModel.hh"

#include "acorn-utils.hh"

extern "C" {
#include <fitsy.h>
}

#define NMap 10

  typedef struct _QEMap {
	int	nwave;
	int     *waves;			// Map from wave # to QE array.
        double  *ratio;
	int      nmap;
	double **maps;			// Array of QE arrays.
  } QEMap;

struct AcornSurfaceQE {
    ACORN_SURFACE

    Param double	nx;
    Param double	ny;
    Param double	sx;
    Param double	sy;

    double		wave[NMap];
    string		mapfile[NMap];

    QEMap	*mapdata;

    AcornSurfaceQE();
};

static std::map<const char *, VarMap> VTable = {
#	include "QE.vtable"
};

static int Traverse(AcornModel *m, AcornSurface *S, AcornRay &r)
{
	AcornSurfaceQE *s = (AcornSurfaceQE *) S;

    int    nx = s->nx;
    int    ny = s->ny;
    double sx = s->sx;
    double sy = s->sy;

    int    qemap = s->mapdata->waves[r.wave];
    double ratio = s->mapdata->ratio[r.wave];

    int cx = (int) (r.p[X]/sx);
    int cy = (int) (r.p[Y]/sy);

    cx += nx/2;
    cy += ny/2;

    int ix = (int) (cx + 0.5);
    int iy = (int) (cy + 0.5);

    if ( ix >= 0 && ix < nx && iy >= 0 && iy < ny ) {
	double    QE1 = s.mapdata->maps[qemap][iy*nx + ix];

	if ( ratio == 1.0 ) {
	    r.intensity *= QE1;
	} else {
	    double    QE2 = s.mapdata->maps[qemap+1][iy*nx + ix];

	    r.intensity *= QE1*ratio + QE2*(1.0-ratio);
	}
    }

    return 0;
}

int qe_init(AcornModel *m, AcornSurface *S) {
	AcornSurfaceQE *s = (AcornSurfaceQE *) S;

	int i, j, nmap;

	char  path[1024];
	char *qemaps = getenv("ACORN_QEMAPS");

    if ( !s.mapdata ) {
	s->mapdata = (QEMap *) calloc(sizeof(QEMap), 1);

	s->mapdata->maps  = (double **) calloc(sizeof(double *), NMap);

	for ( nmap = 0; nmap < NMap; nmap++ ) {
	    if ( s.wave[nmap] == 0.0 ) { break; }

	    path[0] = '\0';
	    if ( qemaps ) {
		strncpy(path, qemaps, sizeof(path)-1);
		strcat(path, "/");
	    }
	    strcat(path, s.mapfile[nmap].c_str());

	    if ( !ft_simpleimageread(path, NULL, (void **) &s->mapdata->maps[nmap], NULL, -64) ) {
		fprintf(stderr, "Cannot load QE map : %s\n", path);
	    }
	    //printf("map %d %f %s\n", nmap, s->wave[nmap], path);
	}

	s->mapdata->nmap = nmap;
    }
	
    if ( s->mapdata->waves ) {
	free(s->mapdata->waves);
	free(s->mapdata->ratio);
    }

    nmap = s->mapdata->nmap;

    s->mapdata->waves = (int      *) calloc(sizeof(int), m->nwave);
    s->mapdata->ratio = (double   *) calloc(sizeof(double), m->nwave);

    s->mapdata->nwave = m->nwave;

    for ( i = 0; i < m->nwave; i++ ) {

	double wave = m->wavelength[i];

        for ( j = 0; j < nmap; j++ ) {
	    if ( wave < s->wave[j] ) { break; }
	}

	if ( j <= 0 ) {
	    s->mapdata->waves[i] = 0;
	    s->mapdata->ratio[i] = 1.0;
	} else {
	    if ( j >= nmap ) {
		s->mapdata->waves[i] = nmap-1;
		s->mapdata->ratio[i] = 1.0;
	    } else {
		j--;

		s->mapdata->waves[i] = j;
		s->mapdata->ratio[i] = 1.0 - (s->wave[j] - wave) / (s->wave[j] - s->wave[j+1]);
	    }
	}

	//fprintf(stderr, "wave %d %f : %d %f\n", i, wave, map->waves[i], map->ratio[i]);
    }


    return 1;
}


AcornSurfaceQE::AcornSurfaceQE() {
    int		nx = 1024;
    int		ny = 1024;
    int		sx = 1.0;
    int		sy = 1.0;

    traverse = Traverse;
    initials = NULL;
    vtable   = &VTable;
}

extern "C" {
    AcornSurface *AcornSurfConstructor()   { return (AcornSurface *) new AcornSurfaceQE(); }
}

