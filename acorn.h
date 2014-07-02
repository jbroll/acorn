
#include "rays.h"

typedef char *string;

#define X__PI	3.14159265358979323846
#define X_2PI	( 2 * X__PI )
#define X_R2D	(X_2PI / 360.0)
#define X_R2H	(X_2PI /  24.0)
#define X_H2D	(360.0 /  24.0)

#define r2h(r)	( (r) / X_R2H )
#define h2r(d)	( (d) * X_R2H )
#define r2d(r)	( (r) / X_R2D )
#define d2r(d)	( (d) * X_R2D )
#define h2d(r)	( (r) * X_H2D )
#define d2h(d)	( (d) / X_H2D )

#define ACORN_ITER	15

// Special surface types
//
#define COORDBK	-1


#define ACORN_PARAMETERS	1
#define ACORN_STRINGS		2
#define ACORN_ANNOTE		3

typedef int (*TraceFunc)(struct _MData *m, struct _Surface *s, struct _Ray *r);
typedef int (*InfosFunc)(int info, char ***str, double **val);

enum PX_Param {
    Px_px, Px_py, Px_pz, Px_rx, Px_ry, Px_rz
  , Px_thickness
  , Px_aper_min, Px_aper_max, Px_aper_xoff, Px_aper_yoff
  , Px_n

  , Px_NParams
};


typedef struct _Surface {
    char*	name;
    char*	type;
    char*	comment;
    TraceFunc   traverse;
    InfosFunc   info;

    char*	aper_type;
    char*	aper_param;

    char*	aperture;
    long	aper_data;
    long	aper_leng;
    
    double	p[256];
    char*	s[32];

    char*	glass;
    void*	glass_ptr;
    double*	indicies;

    long	enable;
    long	annote;
} Surface;

typedef struct _SurfaceList {
    Surface 	*surf;
    int		 nsurf;
    int		 type;
} SurfaceList;

typedef struct _MData {
    double z;
    double *indicies;
    double *wavelength;
} MData;

void aper_init(Surface *s, Affine3d transform);
int  aper_clip(Surface *s, struct _Ray *r);

typedef void (*SagittaFunc)(Surface &s, double x, double y, double *dz, double *dx, double *dy);

extern "C" {
    int  SurfSize(void);
    int  RaysSize(void);
}

