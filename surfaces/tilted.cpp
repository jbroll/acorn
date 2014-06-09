#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"

    enum Px_Local { Pm_TanX = Px_NParams, Pm_TanY };

#include "acorn-utils.h"



extern "C" {


  static const char  *MyParamNames[] = { "tanx", "tany" };
  static const double MyParamValue[] = { 0.0, 0.0 };


  int info(int command, char **strings, double **values) 
  {
    switch ( command ) {
	case ACORN_PARAMETERS: {

	    *strings = (char *)   MyParamNames;
	    *values  = (double *) MyParamValue;

	    return sizeof(MyParamNames)/sizeof(char *);
        }
    }
    return 0;
  }

  int traverse(MData *m, Surface &s, Ray &r)
  {
	double n0 = m->n;
	double  z = m->z;

    double tanx = s.p[Pm_TanX];
    double tany = s.p[Pm_TanY];
    double n    = s.p[Px_n];

    // http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-plane-and-ray-disk-intersection/

    Vector3d nhat = Vector3d(tanx, tany, 1.0).normalized();
    Vector3d l0   = -r.p;

    double d = nhat.dot(r.k);			// Denominator or Distance.

    if ( d < 1e-10 ) 	{ return 1; }

    d = l0.dot(nhat) / d; 

    if ( d < 0 ) 	{ return 1; };

    r.p += d * r.k; 				// Ray/Surface Intersection position

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
  }
}
