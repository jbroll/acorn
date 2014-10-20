#ifndef AcornRay_HH
#define AcornRay_HH

#include <Eigen/Dense>

using namespace Eigen;

#define X	0
#define Y	1
#define Z	2

struct AcornRay {
    Vector3d	p;
    Vector3d	k;
    int		wave;
    int		vignetted;
    float	intensity;
};
#endif
