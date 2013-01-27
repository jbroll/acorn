
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

typedef void (*TraceFunc)(double z, double n, struct _Surface *s, struct _Ray *r);

typedef struct _Surface {
    double	aper;
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

    char*	name;
    char*	type;
    TraceFunc   traverse;
} Surface;

typedef struct _Ray {
    Vector3d	p;
    Vector3d	k;
    int		vignetted;
} Ray;

