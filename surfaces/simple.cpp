#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"
#include "acorn-utils.h"


extern "C" {


    enum Px_Local { Pm_R = Px_NParams, Pm_K };

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

  int traverse(double n0, double z, Surface &s, Ray &r)
  {
    double d;

    double R = s.p[Pm_R];
    double K = s.p[Pm_K];
    double n = s.p[Px_n];

    // Intersect
    //
    // establish sign flippy dealies
    //
    double Ksign = 1.0;
    double Dsign = r.k(Z)/fabs(r.k(Z));
    double Rsign = R/fabs(R);

    Vector3d nhat;

    d = AcornSimpleSurfaceDistance(r, z, R, K);

    // Ray/Surface Intersection position
    //
    r.p += d * r.k;

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

    return 0;
  }
}
