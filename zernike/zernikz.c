#include <stdio.h>

// Converted to C 2013 JBR

/*+
    *KWIC zernik.f
    *
    *$Id: zernik.f,v 1.1 2004/03/16 15:50:17 dtn Exp $ 
    *
    *Revisions:
    *   95-Oct-10[T. Gaetz]
    *      . eliminate output to unit 6
    *   94-Jan-27[T. Gaetz]
    *      . collapse data statements to reduce number of continuations
    *        (the SPARC10 compiler seems to be enforcing the F77 standard
    *        limit of 19 continuations.
    *-
    !
    !  UPDATED 5/2/88 TO
    !    (1) CORRECT A MATH ERROR IN THE DERIVATIVE CALCULATIONS
    !    (2) CORRECT AN INDEXING ERROR THAT PREVENTED THE FULL NUMBER
    !        (325) OF POLYNOMIALS FROM BEING USED
    !    (3) USE FULL DOUBLE PRECISION MATH
    !  PAUL GLENN, BAUER ASSOCIATES, INC.
    !
      SUBROUTINE ZERNIK (NP, XN, YN, ZERP, ZERX, ZERY, IER)
C    ****************************************
C    *
C    *    PERKIN-ELMER CORPORATE COMPUTING
C    *      SOFTWARE ENGINEERING SECTION
C    *
C    *    ZERNIK FORTRAN
C    *    WRITTEN BY VALERIE WALLACE
C    *            ON 10/07/80
C    *
C    *    UPDATE:   11/03/80
C    *    TIME:     15:34:28
C    *
C    ******************************************
C
C  EVALUATE ZERNIKE POLYNOMIALS AT POINT (XN, YN) AND THEIR PARTIAL
C  DERIVATIVES. THE MAXIMUM NUMBER OF TERMS ALLOWED (NMAX) IS 325
C  WHICH ALLOWS 20 RADIAL DEGREES OF FREEDOM.
C
C  INPUT ARGUMENTS:
C    NP : I*4 - NUMBER OF ZERNIKE TERMS DESIRED
C    XN : R*4 - NORMALIZED X COORD. = X / R2S = R * COS (THETA)
C    YN : R*4 - NORMALIZED Y COORD. = Y / R2S = R * SIN (THETA)
C
C  OUTPUT ARGUMENTS:
C    ZERP : R*4 - ARRAY (NP) VALUES OF ZERNIKE POLYNOMIALS
C    ZERX : R*4 - ARRAY (NP) OF 1/2 THE PARTIAL DERIVATIVES OF ZERP WRT
C                 XN
C    ZERY : R*4 - ARRAY (NP) OF 1/2 THE PARTIAL DERIVATIVES OF ZERP WRT
C                 YN
C    IER  : I*4 - ERROR CODE
C         : 0 - NO ERROR
C         : 1 - (XN, YN) IS OUTSIDE THE UNIT DISK
C         : 2 - 0 < OR = NP < 3 - WARNING, NP SET TO 3
C         : 4 - NP > NMAX - WARNING, NP RESET TO NMAX = 325
C         : 6 - NP < 0 - FATAL ERROR
C         : OTHERWISE - COMBINATION OF ABOVE ERRORS
C
C  EXTERNAL REFERENCES: INCLIN
C
C  CS : R ** M * COS (M * THETA)
C  CX : PARTIAL OF CS WRT X
C  CY : PARTIAL OF CS WRT Y
C  SS : R ** M * SIN (M * THETA)
C  SX : PARTIAL OF SS WRT X
C  SY : PARTIAL OF SS WRT Y
C       WHERE M = N - 1
C
*/
int zernikz_std(double XN, double YN, int NP, double *c, double *z, double *dx, double *dy)
{
      
    //IMPLICIT DOUBLE PRECISION (A-H,O-Z)
    //IMPLICIT INTEGER (I-N)

    double cx[25], cy[25], cs[25], sx[25], sy[25], ss[25], zerp[325], zerx[325], zery[325];
    double DUX, DUY, SUM;

    //  SQRTN : SQRT (N)
    //  (USE FULL DOUBLE PRECISION)
    //
    double sqrt[] = {
         1.00000000000000000000e+00,    1.41421356237309500000e+00,
         1.73205080756887700000e+00,    2.00000000000000000000e+00,
         2.23606797749979000000e+00,    2.44948974278317800000e+00,
         2.64575131106459100000e+00,    2.82842712474619000000e+00,
         3.00000000000000000000e+00,    3.16227766016838000000e+00,
         3.31662479035540000000e+00,    3.46410161513775400000e+00,
         3.60555127546398900000e+00,    3.74165738677394100000e+00,
         3.87298334620741700000e+00,    4.00000000000000000000e+00,
         4.12310562561766100000e+00,    4.24264068711928500000e+00,
         4.35889894354067400000e+00,    4.47213595499958000000e+00,
         4.58257569495584000000e+00,    4.69041575982343000000e+00,
         4.79583152331271900000e+00,    4.89897948556635600000e+00,
         5.00000000000000000000e+00 };

    //  FACT (N) : (N-1)]
    //  (USE FULL DOUBLE PRECISION)
    //
    double fact[] = {
         1.00000000000000000000e+00,    1.00000000000000000000e+00,
         2.00000000000000000000e+00,    6.00000000000000000000e+00,
         2.40000000000000000000e+01,    1.20000000000000000000e+02,
         7.20000000000000000000e+02,    5.04000000000000000000e+03,
         4.03200000000000000000e+04,    3.62880000000000000000e+05,
         3.62880000000000000000e+06,    3.99168000000000000000e+07,
         4.79001600000000000000e+08,    6.22702080000000000000e+09,
         8.71782912000000000000e+10,    1.30767436800000000000e+12,
         2.09227898880000000000e+13,    3.55687428096000000000e+14,
         6.40237370572800000000e+15,    1.21645100408832000000e+17,
         2.43290200817664000000e+18,    5.10909421717094400000e+19,
         1.12400072777760800000e+21,    2.58520167388849800000e+22,
         6.20448401733239400000e+23,    1.55112100433309900000e+25 };


    // Adjust pointers for FORTRAN indexing
    //
    double *FACT  = fact - 1;
    double *SQRT  = sqrt - 1;
    double *CX = cx - 1;
    double *CY = cy - 1;
    double *CS = cs - 1;
    double *SX = sx - 1;
    double *SY = sy - 1;
    double *SS = ss - 1;

    double Y2, X2, R2;
    double COS2TH, SIN2TH, TERM;

    int N, L, IP, M, MS, NMM, NMS, NPM;


    //  INITIALIZE NMAX, IER
    //
    int NMAX = 325;
    int IER  = 0;

    if ( NP < 3    ) { IER = 2; NP = 3;	  }
    if ( NP > NMAX ) { IER = 4; NP = NMAX; }

    if ( XN * XN + YN * YN > 1 ) { IER = IER + 1; }

    //  EVALUATE ZERNIK POLYNOMIALS AND THEIR PARTIAL DERIVATIVES.
    //
    CX[1] = 0.0;
    CY[1] = 0.0;
    CS[1] = 1.0;
    CX[2] = 0.5;
    CY[2] = 0.0;
    CS[2] = XN;
    SX[1] = 0.0;
    SY[1] = 0.0;
    SS[1] = 0.0;
    SX[2] = 0.0;
    SY[2] = 0.5;
    SS[2] = YN;
    Y2 = YN * YN;
    X2 = XN * XN;
    R2 = X2 + Y2;
    COS2TH = X2 - Y2;
    SIN2TH = 2.0 * XN * YN;

    //  EVALUATE FIRST THREE POLYNOMIALS
    //
    c--;

    *z  = c[1] * 1.0;
    *z += c[2] * 2.0 * XN;
    *z += c[3] * 2.0 * YN;

    //  EVALUATE ONE HALF OF THE PARTIAL DERIVATIVES OF THE FIRST
    //  THREE POLYNOMIALS.
    //
    *dx  = c[1] * 2.0 * 0.0;
    *dy  = c[1] * 2.0 * 0.0;
    *dy += c[2] * 2.0 * 0.0;
    *dx += c[2] * 2.0 * 1.0;
    *dx += c[3] * 2.0 * 0.0;
    *dy += c[3] * 2.0 * 1.0;

    IP = 3;
    MS = 2;

    //  EVALUATE REMAINING TERMS
    //
    N = 3;


    while ( IP < NP ) {
      CS[N] = CS[N - 2] * COS2TH - SS[N - 2] * SIN2TH;
      CX[N] = CX[N - 2] * COS2TH + CS[N - 2] * XN - SX[N - 2] * SIN2TH - SS[N - 2] * YN;
      CY[N] = CY[N - 2] * COS2TH - CS[N - 2] * YN - SY[N - 2] * SIN2TH - SS[N - 2] * XN;

      SS[N] = SS[N - 2] * COS2TH + CS[N - 2] * SIN2TH;
      SX[N] = SX[N - 2] * COS2TH + SS[N - 2] * XN + CX[N - 2] * SIN2TH + CS[N - 2] * YN;
      SY[N] = SY[N - 2] * COS2TH - SS[N - 2] * YN + CY[N - 2] * SIN2TH + CS[N - 2] * XN;

      MS = 3 - MS;

      for ( M = MS; M <= N; M += 2 ) {
        if ( IP > NP ) { goto done; }

            NMM = (N - M) / 2;
            NPM = NMM + M;
            TERM = FACT[N] / (FACT[NMM + 1] * FACT[NPM]);
            SUM = TERM;
            DUX = 0.0;
            DUY = 0.0;

            if ( NMM > 0 ) {
		for ( L = 1; L <= NMM; L++ ) {
		    TERM = (NMM + 1 - L) * (NPM - L) * TERM / (L * (L - N));
		    DUX = R2 * DUX + XN * SUM;
		    DUY = R2 * DUY + YN * SUM;
		    SUM = R2 * SUM + TERM;
		}
	    }

	    SUM = SUM * SQRT[N];
            DUX = DUX * SQRT[N];
            DUY = DUY * SQRT[N];

            if ( M <= 1 ) {
		IP = IP + 1;
		if ( IP > NP ) { goto done; }

		//  USE COSINE TERM
		//
		*z  += c[IP] * SUM * CS[M];
		*dx += c[IP] * 2.0 * ( DUX * CS[M] + SUM * CX[M] );
		*dy += c[IP] * 2.0 * ( DUY * CS[M] + SUM * CY[M] );
	    } else {
		IP = IP + 1;
		if ( IP > NP ) { goto done; }

		SUM = SUM * SQRT[2];
		DUX = DUX * SQRT[2];
		DUY = DUY * SQRT[2];


		//  USE SINE TERM
		//
		*z  += c[IP] * SUM * SS[M];
		*dx += c[IP] * 2.0 * ( DUX * SS[M] + SUM * SX[M] );
		*dy += c[IP] * 2.0 * ( DUY * SS[M] + SUM * SY[M] );

		IP = IP + 1;
		if ( IP > NP ) { goto done; }


		//  USE COSINE TERM
		//
		*z  += c[IP] * SUM * CS[M];
		*dx += c[IP] * 2.0 * ( DUX * CS[M] + SUM * CX[M] );
		*dy += c[IP] * 2.0 * ( DUY * CS[M] + SUM * CY[M] );
	    }
        }

        N = N + 1;
    }
    done:

   return IER;
}
