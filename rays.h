
#define X	0
#define Y	1
#define Z	2

typedef struct _Ray {
    Vector3d	p;
    Vector3d	k;
    int		wave;
    int		vignetted;
    float	intensity;
} Ray;

