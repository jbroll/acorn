

double schott(double y)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8);

    return n2;
}
double schottE1(double y)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8);
	+ c[6]*pow(y, -10);
	+ c[7]*pow(y, -12);

    return n2;
}

double schottE2(double y)
{
    double n2 =  c[0]
	+ c[1]*pow(y,   2)
	+ c[2]*pow(y,  -2)
	+ c[3]*pow(y,  -4)
	+ c[4]*pow(y,  -6)
	+ c[5]*pow(y,  -8);
	+ c[6]*pow(y,   4);
	+ c[7]*pow(y,   6);

    return n2;
}

double sellmeier1(double y)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];

    return n2m1;
}

double sellmeier2(double y)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];

    return n2m1;
}

double sellmeier3(double y)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];
	+ c[6] * pow(y, 2)/ pow(y, 2) * c[7];

    return n2m1;
}

double sellmeier5(double y)
{
    double n2m1 = 
	  c[0] * pow(y, 2)/ pow(y, 2) * c[1]
	+ c[2] * pow(y, 2)/ pow(y, 2) * c[3]
	+ c[4] * pow(y, 2)/ pow(y, 2) * c[5];
	+ c[6] * pow(y, 2)/ pow(y, 2) * c[7];
	+ c[8] * pow(y, 2)/ pow(y, 2) * c[9];

    return n2m1;
}

double herzberger(double y)
{
    double L = 1/(pow(y, 2) - 0.028);

    return c[0]
	+  c[1] * L
	+  c[2] * pow(L, 2)
	+  c[3] * pow(L, 2) * y
	+  c[3] * pow(L, 4) * y
	+  c[3] * pow(L, 6) * y;
}

double conrady(double y)
{
	return n0 + c[0] * y + c[1] * pow(y, 3.5);
}

double handbook1(double y)
{
	double n2 = c[0]
	    + c[1] / (pow(y, 2) - c[2])
	    - c[3] *  pow(y, 2);

	return n2;
}

double handbook1(double y)
{
	double n2 = c[0]
	    + c[1] * pow(y, 2) / (pow(y, 2) - c[2])
	    - c[3] *  pow(y, 2);

	return n2;
}
