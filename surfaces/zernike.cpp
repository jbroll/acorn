/*     SUBROUTINE ICZERN (R2, K, P, TEL, KODE)
C
C   /****************************************
C    *
C    *    PERKIN-ELMER CORPORATE COMPUTING
C    *      SOFTWARE ENGINEERING SECTION
C    *
C    *    ICZERN FORTRAN
C    *    WRITTEN BY H. JACKSON  M/S 180  X (203) 762-4395
C    *            ON 07/01/80
C    *
C    *    UPDATE:   10/22/80
C    *    TIME:     09:01:06
C    *
C   ITERATIVELY CORRECT FOR THE ZERNIKE DEFORMATIONS
C   OF AN OPTICAL SURFACE
C 
C    ******************************************/

#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#include "../acorn.h"
#include "acorn-utils.h" 

extern "C" {
#include "../zernike/zernike.h"

    enum Px_Local { Pm_R = Px_NParams, Pm_K, Pm_xdecenter, Pm_ydecenter, Pm_nradius, Pm_nzterms, Pm_z1 };

  static const char *MyParamNames[]   = { "R", "K"
          , "xdecenter" , "ydecenter"	, "nradius" , "nzterms"
	  ,         "z1",  "z2",  "z3",  "z4",  "z5",  "z6",  "z7",  "z8",  "z9"
	  , "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19"
	  , "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29"
	  , "z30", "z31", "z32", "z33", "z34", "z35", "z36", "z37", "z38", "z39"
	  , "z40", "z41", "z42", "z43", "z44", "z45", "z46", "z47", "z48", "z49"
	  , "z50", "z51", "z52", "z53", "z54", "z55", "z56", "z57", "z58", "z59"
	  , "z60", "z61", "z62", "z63", "z64", "z65", "z66", "z67", "z68", "z69"
	  , "z70", "z71", "z72", "z73", "z74", "z75", "z76", "z77", "z78", "z79"
	  , "z80", "z81", "z82", "z83", "z84", "z85", "z86", "z87", "z88", "z89"
	  , "z90", "z91", "z92", "z93", "z94", "z95", "z96", "z97", "z98", "z99"
	  , "z100", "z101", "z102", "z103"
  };
  static const double MyParamValue[] = {  0.0, 0.0
           , 0.0, 0.0, 0.0, 1.0
           , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      , 0.0, 0.0, 0.0, 0.0
  };


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
	double n = s.p[Px_n];

        double R2= s.p[Pm_R];	//    R2   : R*8 - CONSTANT TERM OF SURFACE EQUATION
	double K = 0.0;		//    K    : R*8 - 1 / 2 LINEAR TERM IN SURFACE EQUATION
	double P = s.p[Pm_K];	//    P    : R*8 - CONIC CONSTANT = 1 - ECCENTRICITY ** 2

	double    XD = r.p(X);	//   : R*8 - X COORD. OF RAY INTERSECTION WITH UNDEFORMED SURFACE
	double    YD = r.p(Y);	//   : R*8 - Y COORD. OF RAY INTERSECTION WITH UNDEFORMED SURFACE
	double    ZD = r.p(Z);	//   : R*8 - Z COORD. OF RAY INTERSECTION WITH UNDEFORMED SURFACE
	double    XL = r.k(X);	//   : R*8 - X COMPONENT OF INPUT DIRECTION
	double    YL = r.k(Y);	//   : R*8 - Y COMPONENT OF INPUT DIRECTION
	double    ZL = r.k(Z);	//   : R*8 - Z COMPONENT OF INPUT DIRECTION

	double    XG = 0;	//   : R*8 - X COMPONENT OF GRADIENT OF SURFACE AT INTERSECTION
	double    YG = 0;	//   : R*8 - Y COMPONENT OF GRADIENT OF SURFACE AT INTERSECTION
	double    ZG = 0;	//   : R*8 - Z COMPONENT OF GRADIENT OF SURFACE AT INTERSECTION

	double	  TEL;		//   : R*8 - TOTAL DISTANCE CORRECTION
	double	  DEF2 = 0; 	//   : R*8 - SUM OF THE SQUARES OF THE DEFORMATION COEFFICIENTS


	double F, FDOT, RCS, ZF;
	double XAP, YAP, ZAP;

	double RHOS = XD*XD + YD*YD;
	double PZK = P * ZD - K;
	double DEL = RHOS + PZK*PZK + ZD*ZD + 1.0;
	double TS = DEL * 1.0E-14 + DEF2 * 1.0E-05;

	double xdecenter = s.p[Pm_xdecenter];
	double ydecenter = s.p[Pm_ydecenter];
	double   nradius = s.p[Pm_nradius];
	int      nzterms = s.p[Pm_nzterms];

	int IT = 0;

	for ( int i = 0; i < nzterms; i++ ) { DEF2 += s.p[Pm_z1+i]*s.p[Pm_z1+i]; }

	while ( 1 ) {

	    zernike_std((XD + xdecenter)/nradius, (YD + ydecenter)/nradius, nzterms, &s.p[Pm_z1], &ZAP, &XAP, &YAP);

	    ZF = ZD - ZAP;
	    PZK = P*ZF - K;
	    XG = XD - XAP * PZK / nradius;
	    YG = YD - YAP * PZK / nradius;
	    ZG = PZK;
	    RCS = R2 + ZF * (K - PZK);
	    F = RHOS - RCS;

	    if ( fabs(F) < TS ) { 		    break;	}	// DONE

	    if ( ++IT    > 5  ) {		    return 9;	}	// KODE = 9 : TOO MANY ITERATIONS

	    FDOT = XG * XL + YG * YL + ZG * ZL;

	    printf("%f %f\n", FDOT*DEL, F+F);

	    if ( fabs(FDOT * DEL) < fabs(F + F) ) { return 8;	} 	// KODE = 8 : EXCESSIVE DEFORMATION OR ZERO DIVIDE

	    //  EVALUATE AND USE DEL
	    //
	    DEL = - F / (FDOT + FDOT);
	    TEL = TEL + DEL;
	    XD = XD + DEL * XL;
	    YD = YD + DEL * YL;
	    ZD = ZD + DEL * ZL;
	    RHOS = XD*XD + YD*YD;
	}

	return 0;
}
}
