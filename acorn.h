
#define X	0
#define Y	1
#define Z	2

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

// Special surface types
//
#define COORDBK	-1


#define ACORN_PARAMETERS	1

typedef void (*TraceFunc)(double z, double n, struct _Surface *s, struct _Ray *r);
typedef int  (*InfosFunc)(int info, char **str, double **val);

typedef struct _Surface {
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

    char*	aper_type;
    double	aper_min;
    double	aper_max;
    string	aper_param;

    char*	aperture;
    long	aper_data;
    long	aper_leng;
    
    double	p[256];
    char*	s[32];

    char*	name;
    char*	type;
    TraceFunc   traverse;
    InfosFunc   info;

    char*	glass;
    void*	glass_ptr;
} Surface;

typedef struct _SurfaceList {
    Surface 	*surf;
    int		 nsurf;
    int		 type;
} SurfaceList;

typedef struct _Ray {
    Vector3d	p;
    Vector3d	k;
    int		vignetted;
} Ray;

extern "C" {
    int  SurfSize(void);
    int  RaysSize(void);

    void xrays(Ray *r, int n);
    void prays(Ray *r, int n);

    void aper_init(Surface *s, Affine3d transform);
    int  aper_clip(Surface *s, Ray *r);
}
