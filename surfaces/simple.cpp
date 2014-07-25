#include <stdio.h>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"

    enum Px_Local { Pm_R = Px_NParams, Pm_K };

#include "acorn-utils.h"


extern "C" {


  static const char  *MyParamNames[] = { "R", "K" };
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
	double n0 = m->indicies[r.wave];
	double  z = m->z;

    double d;

    double R = s.p[Pm_R];
    double K = s.p[Pm_K];
    double n = s.indicies[r.wave];



    Vector3d nhat;

    //
    d = AcornSimpleSurfaceDistance(r, z, R, K);
    r.p += d * r.k; 				// Ray/Surface Intersection position

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
  }
}
