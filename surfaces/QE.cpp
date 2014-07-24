#include <stdio.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <sys/mman.h>

#include "../acorn.h"
#include "/data/mmti/src/fitsy/fitsy.h"

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

  static QEMap *QEInit(MData *m, Surface &s) {
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
	    if ( qemaps ) { strncpy(path, qemaps, sizeof(path)-1); }
	    strcat(path, "/");
	    strcat(path, s.s[nmap]);

	    if ( !ft_simpleimageread(path, NULL, (void **) &map->maps[nmap], NULL, -64) ) {
		fprintf(stderr, "Cannot load QE map : %s\n", path);
	    }
	    //printf("map %d %p\n", nmap, map->maps[nmap]);
	}

	map->nmap = nmap;
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
		map->ratio[i] = (s.p[j+Pm_w0] - wave) / (s.p[j+Pm_w0] - s.p[j+Pm_w0+1]);
	    }
	}
    }


    return map;
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
    int    nx = s.p[Pm_nx];
    int    ny = s.p[Pm_ny];
    double sx = s.p[Pm_sx];
    double sy = s.p[Pm_sy];

    QEMap *map = (QEMap *) s.data;

    if ( !map || map->nwave != m->nwave ) { 
	s.data = (void *) QEInit(m, s);
	map = (QEMap *) s.data;
    }

    int    qemap = map->waves[r.wave];
    double ratio = map->ratio[r.wave];

    int cx = r.p[X]/sx;
    int cy = r.p[Y]/sy;

    cx += nx/2;
    cy += ny/2;

    int ix = cx + 0.5;
    int iy = cy + 0.5;

    if ( ix >= 0 && ix < nx && iy >= 0 && iy < ny ) {
	double    QE = map->maps[qemap][iy*nx + ix];

	//printf("%d %d %f\n", ix, iy, QE);

	r.intensity += 1*QE;
    }

    return 0;
  }
}
