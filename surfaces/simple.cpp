#include <stdio.h>

#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"
#include "acorn-utils.h"


extern "C" {

    void prays(Ray *ray, int n)
    {
	for ( int i = 0; i < n; i++ ) {
	    printf("%5d\t%10.6f\t%10.6f\t%10.6f\t", i, ray[i].p(X), ray[i].p(Y), ray[i].p(Z));
	    printf("%10.6f\t%10.6f\t%10.6f\t%d\n",     ray[i].k(X), ray[i].k(Y), ray[i].k(Z), ray[i].vignetted);
	}
    }

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

    if ( aper_clip(&s, &r) ) { return 1; }

    nhat = AcornSimpleSurfaceNormal(r, R, K);

    AcornRefract(r, nhat, n0, n);		// Reflect or Refract

//prays(&r, 1);

    return 0;
  }
}
