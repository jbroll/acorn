#include <stdio.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <sys/mman.h>

#include "../acorn.h"
#include <fitsy.h>

#include "acorn-utils.h"


extern "C" {

  enum Px_Local { Pm_nx = Px_NParams, Pm_ny, Pm_sx, Pm_sy, Pm_w0 };

#define NMap 10

  static const char  *MyParamNames[] = { "nx", "ny", "sx", "sy", "w0", "w1", "w2", "w3",  "w4", "w5", "w6", "w7", "w8", "w9" };
  static const double MyParamValue[] = { 1024.0, 1024.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  static const char  *MyStringNames[] = { "map0", "map1", "map2", "map3",  "map4", "map5", "map6", "map7", "map8", "map9" };
  static const char  *MyStringValue[] = { "", "", "", "",  "", "", "", "", "", "" };

  typedef struct _QEMap {
	int	nwave;
	int     *waves;			// Map from wave # to QE array.
        double  *ratio;
	int      nmap;
	double **maps;			// Array of QE arrays.
  } QEMap;

  int inits(MData *m, Surface &s, Ray &ray) {
	int i, j, nmap;

	char  path[1024];
	char *qemaps = getenv("ACORN_QEMAPS");
	QEMap *map = (QEMap *) s.data;

    if ( !map ) {
	map = (QEMap *) calloc(sizeof(QEMap), 1);

	map->maps  = (double **) calloc(sizeof(double *), NMap);

	for ( nmap = 0; nmap < NMap; nmap++ ) {
	    if ( s.p[nmap+Pm_w0] == 0.0 ) { break; }

	    path[0] = '\0';
	    if ( qemaps ) {
		strncpy(path, qemaps, sizeof(path)-1);
		strcat(path, "/");
	    }
	    strcat(path, s.s[nmap]);

	    if ( !ft_simpleimageread(path, NULL, (void **) &map->maps[nmap], NULL, -64) ) {
		fprintf(stderr, "Cannot load QE map : %s\n", path);
	    }
	    //printf("map %d %f %s\n", nmap, s.p[nmap+Pm_w0], path);
	}

	map->nmap = nmap;

	s.data = (void *) map;
    }
	
    if ( map->waves ) {
	free(map->waves);
	free(map->ratio);
    }

    nmap = map->nmap;

    map->waves = (int      *) calloc(sizeof(int), m->nwave);
    map->ratio = (double   *) calloc(sizeof(double), m->nwave);

    map->nwave = m->nwave;

    for ( i = 0; i < m->nwave; i++ ) {

	double wave = m->wavelength[i];

        for ( j = 0; j < nmap; j++ ) {
	    if ( wave < s.p[j+Pm_w0] ) { break; }
	}

	if ( j <= 0 ) {
	    map->waves[i] = 0;
	    map->ratio[i] = 1.0;
	} else {
	    if ( j >= nmap ) {
		map->waves[i] = nmap-1;
		map->ratio[i] = 1.0;
	    } else {
		j--;

		map->waves[i] = j;
		map->ratio[i] = 1.0 - (s.p[j+Pm_w0] - wave) / (s.p[j+Pm_w0] - s.p[j+Pm_w0+1]);
	    }
	}

	//fprintf(stderr, "wave %d %f : %d %f\n", i, wave, map->waves[i], map->ratio[i]);
    }


    return 1;
  }

  int info(int command, char **strings, double **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {

	    *strings = (char   *) MyParamNames;
	    *values  = (double *) MyParamValue;

	    return sizeof(MyParamNames)/sizeof(char *);
        }
	case ACORN_STRINGS: { 
	    *strings = (char  *) MyStringNames;
	    *values  = (double*) MyStringValue;

	    return sizeof(MyStringNames)/sizeof(char *);
	}
    }
    return 0;
  }

  int traverse(MData *m, Surface &s, Ray &r)
  {
    int    nx = (int) s.p[Pm_nx];
    int    ny = (int) s.p[Pm_ny];
    double sx = s.p[Pm_sx];
    double sy = s.p[Pm_sy];

    QEMap *map = (QEMap *) s.data;

    int    qemap = map->waves[r.wave];
    double ratio = map->ratio[r.wave];

    int cx = (int) (r.p[X]/sx);
    int cy = (int) (r.p[Y]/sy);

    cx += nx/2;
    cy += ny/2;

    int ix = (int) (cx + 0.5);
    int iy = (int) (cy + 0.5);

    if ( ix >= 0 && ix < nx && iy >= 0 && iy < ny ) {
	double    QE1 = map->maps[qemap][iy*nx + ix];

	if ( ratio == 1.0 ) {
	    r.intensity *= QE1;
	} else {
	    double    QE2 = map->maps[qemap+1][iy*nx + ix];

	    r.intensity *= QE1*ratio + QE2*(1.0-ratio);
	}

    }

    return 0;
  }
}
