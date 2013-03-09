
#include <math.h>

#define N0	1

#define Schott	       1
#define Sellmeier1     2 
#define Herzberger     3 
#define Sellmeier2     4 
#define Conrady        5 
#define Sellmeier3     6 
#define Handbook1      7 
#define Handbook2      8 
#define Sellmeier4     9 
#define SchottE1      10
#define Sellmeier5    11
#define SchottE2      12
#define SchottE3      13




double schott(double y, double t, double p, double *c)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8);

    return sqrt(n2);
}
double schottE1(double y, double t, double p, double *c)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8)
	+ c[6]*pow(y, -10)
	+ c[7]*pow(y, -12);

    return sqrt(n2);
}

double schottE2(double y, double t, double p, double *c)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8)
	+ c[6]*pow(y,   4)
	+ c[7]*pow(y,   6);

    return sqrt(n2);
}

double schottE3(double y, double t, double p, double *c)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,   4)
	+ c[3]*pow(y,  -2)
	+ c[4]*pow(y,  -4)
	+ c[5]*pow(y,  -6)
	+ c[6]*pow(y,  -8)
	+ c[7]*pow(y, -10)
	+ c[8]*pow(y, -12);

    return sqrt(n2);
}

double sellmeier1(double y, double t, double p, double *c)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];

    return sqrt(n2m1)+1.0;
}

double sellmeier2(double y, double t, double p, double *c)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];

    return sqrt(n2m1)+1;
}

double sellmeier3(double y, double t, double p, double *c)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5]
	+ c[6] * pow(y, 2)/ pow(y, 2) * c[7];

    return sqrt(n2m1)+1.0;
}

double sellmeier4(double y, double t, double p, double *c)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5]
	+ c[6] * pow(y, 2)/ pow(y, 2) * c[7];

    return sqrt(n2m1)+1.0;
}

double sellmeier5(double y, double t, double p, double *c)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5]
	+ c[6] * pow(y, 2)/ pow(y, 2) * c[7]
	+ c[8] * pow(y, 2)/ pow(y, 2) * c[9];

    return sqrt(n2m1)+1.0;
}

double herzberger(double y, double t, double p, double *c)
{
    double L = 1/(pow(y, 2) - 0.028);

    return c[0]
	+  c[1] * L
	+  c[2] * pow(L, 2)
	+  c[3] * pow(L, 2) * y
	+  c[3] * pow(L, 4) * y
	+  c[3] * pow(L, 6) * y;
}

double conrady(double y, double t, double p, double *c)
{
	return N0 + c[0] * y + c[1] * pow(y, 3.5);
}

double handbook1(double y, double t, double p, double *c)
{
	double n2 = c[0]
	    + c[1] / (pow(y, 2) - c[2])
	    - c[3] *  pow(y, 2);

	return sqrt(n2);
}

double handbook2(double y, double t, double p, double *c)
{
	double n2 = c[0]
	    + c[1] * pow(y, 2) / (pow(y, 2) - c[2])
	    - c[3] *  pow(y, 2);

	return sqrt(n2);
}


double glass_index(int formula, double wave, double temp, double pres, double *c)
{
    switch ( formula ) {
	case Schott	:	return schott    (wave, temp, pres, c);
	case Sellmeier1	:	return sellmeier1(wave, temp, pres, c);
	case Herzberger :	return herzberger(wave, temp, pres, c);
	case Sellmeier2 :	return sellmeier2(wave, temp, pres, c);
	case Conrady    :	return conrady   (wave, temp, pres, c);
	case Sellmeier3 :	return sellmeier3(wave, temp, pres, c);
	case Handbook1  :	return handbook1 (wave, temp, pres, c);
	case Handbook2  :	return handbook2 (wave, temp, pres, c);
	case Sellmeier4 :	return sellmeier4(wave, temp, pres, c);
	case SchottE1  :	return schottE1  (wave, temp, pres, c);
	case Sellmeier5 :	return sellmeier5(wave, temp, pres, c);
	case SchottE2  :	return schottE2  (wave, temp, pres, c);
	case SchottE3  :	return schottE3  (wave, temp, pres, c);
    }

	return 0.0;
}


