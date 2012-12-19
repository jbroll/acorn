
typedef struct _Surface {
    double	aper;
    double	R;
    double	K;
    double	n;
    double 	thickness;
    Affine3d	transform;
    Affine3d	inverse;
} Surface;

typedef struct _Ray {
    int		vignetted;
    Vector3d	p;
    Vector3d	k;
} Ray;

#define X	0
#define Y	1
#define Z	2

