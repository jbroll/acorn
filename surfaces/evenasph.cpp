#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include <stdio.h>

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

  static const char  *MyParamNames[] = { "R", "K", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8" };
  static const double MyParamValue[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


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



    // Normal
    //
    if ( R == 0.0 || abs(R) > 1.0e10 ) {		// Planar
	nhat = Vector3d(0.0, 0.0, -Dsign*1.0);
    } else {
	nhat = Vector3d(Rsign*Dsign*r.p(X), Rsign*Dsign*r.p(Y), -Dsign * sqrt(R * R - (K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y))));
	nhat /= nhat.norm();
    }

    //prays(&r, 1);

    if      ( n == -1 ) {			// Reflect
					    	// http://http.developer.nvidia.com/Cg/reflect.html
	    r.k = r.k - 2 * nhat * nhat.dot(r.k);
    } else if ( n0 != n ) {			 // Refract
						// http://http.developer.nvidia.com/Cg/refract.html
	//printf("Index %f %f\n", n0, n);

	double eta = n0/n;
	double cosi = (-r.k).dot(nhat);
	double cost = 1.0 - eta*eta * ( 1.0 - cosi*cosi);

	r.k = (eta*r.k + (eta*cosi - sqrt(abs(cost))) * nhat) * (cost > 0);
    }
    //prays(&r, 1);

    return 0;
  }
}
