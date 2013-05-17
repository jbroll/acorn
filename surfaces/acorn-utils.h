

inline double AcornSimpleSurfaceDistance(Ray &r, double z, double R, double K)
{
    double d;

    if ( R == 0.0 )  {				// Planar
	d = (z - r.p(Z))/r.k(Z);
    } else {					// http://www-physics.ucsd.edu/~tmurphy/astr597/exercises/raytrace-3d.pdf
	double Ksign = 1.0;
	double Dsign = r.k(Z)/fabs(r.k(Z));
	double Rsign = R/fabs(R);

	if ( K < -1.0 ) { Ksign = -1.0; }

	// solve intersection
	//
	double denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y);

	if (K == -1.0 and denom == 0.0) {	// Special case : Parabola straight in.

	    d = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) - 2*R*(r.p(Z) - z))/(2*R*r.k(Z));
	} else {
	    denom = r.k(X)*r.k(X) + r.k(Y)*r.k(Y) + (K + 1) * r.k(Z)*r.k(Z);

	    double b = (r.p(X)*r.k(X) + r.p(Y)*r.k(Y) + ((K+1)*(r.p(Z)-z) - R)*r.k(Z))/denom;
	    double c = (r.p(X)*r.p(X) + r.p(Y)*r.p(Y) +  (K+1)*(r.p(Z)*r.p(Z) - 2*r.p(Z)*z + z * z)
			    - 2 * R * (r.p(Z)-z))/denom;
	    d = -b - Dsign * Rsign * Ksign * sqrt(b*b - c);
	}
    }

    return d;
}


inline Vector3d AcornSimpleSurfaceNormal(Ray &r, double R, double K)
{
    double Dsign = r.k(Z)/fabs(r.k(Z));
    Vector3d nhat;

    // Normal
    //
    if ( R == 0.0 || abs(R) > 1.0e10 ) {	// Planar
	nhat = Vector3d(0.0, 0.0, -Dsign*1.0);
    } else {
	double Ksign = 1.0;
	double Rsign = R/fabs(R);

	nhat = Vector3d(Rsign*Dsign*r.p(X), Rsign*Dsign*r.p(Y), -Dsign * sqrt(R * R - (K+1)*(r.p(X) * r.p(X) + r.p(Y) * r.p(Y))));
	nhat /= nhat.norm();
    }

    return nhat;
}


inline void AcornRefract(Ray &r, Vector3d &nhat, double n0, double n)
{
    if      ( n == -1 ) {			// Reflect
					    	// http://http.developer.nvidia.com/Cg/reflect.html
	//printf("nhat %f %f %f\n", n0, r.k(X), r.k(Y), r.k(Z));
	//printf("nhat %f %f %f\n", n0, nhat(X), nhat(Y), nhat(Z));

	    r.k = r.k - 2 * nhat * nhat.dot(r.k);
    } else if ( n0 != n ) {			 // Refract
						// http://http.developer.nvidia.com/Cg/refract.html
	//printf("Index %f %f\n", n0, n);

	double eta = n0/n;
	double cosi = (-r.k).dot(nhat);
	double cost = 1.0 - eta*eta * ( 1.0 - cosi*cosi);

	r.k = (eta*r.k + (eta*cosi - sqrt(abs(cost))) * nhat) * (cost > 0);
    }
}

inline int AcornSimpleIterativeIntersect(Surface &s, Ray &r, double z, Vector3d &nhat, SagittaFunc sagitta) 
{
    int i;

    double R = s.p[Pm_R];
    double K = s.p[Pm_K];
    double n = s.p[Px_n];

    double Dsign = r.k(Z)/fabs(r.k(Z));
    double Rsign = R/fabs(R);

    double d = AcornSimpleSurfaceDistance(r, z, R, K); 		// Ray/Surface intersection position
    r.p += d * r.k;

    double Az  = r.p(Z);
    double sag = 0.0;

    double tol = 1e-8;

    for ( i = 0; i < ACORN_ITER; i++ ) {
	double zdx, zdy, zdz;

	sagitta(s, r.p(X), r.p(Y), &zdz, &zdx, &zdy);

	Vector3d P = Vector3d(r.p(X), r.p(Y), Az-zdz);		// Estimate point on the surface.

								// Compute the normal to the conic + deformation
	if ( R == 0.0 || abs(R) > 1.0e10 ) {			// Planar
	    nhat = Vector3d(zdx, zdy, -Dsign*1.0);
	} else {
	    double cdz = sqrt(R * R - (K+1)*(P(X) * P(X) + P(Y) * P(Y)));
	    double cdx = P(X)/cdz;				// These must be slopes to add with zernike slopes.
	    double cdy = P(Y)/cdz;

	    nhat = Vector3d(Rsign*Dsign*(cdx + zdx), Rsign*Dsign*(cdy + zdy), -Dsign*1);
	}
	nhat /= nhat.norm();

	//printf("%d %.20f %.20f\n", i, sag, zdz);
	//fflush(stdout);

	if ( fabs(sag - zdz) < tol ) { break; }
	sag = zdz;

	double Num = (P-r.p).dot(nhat);				// Compute the distance to the surface normal to nhat.
	double Den =     r.k.dot(nhat);

	if ( Den != 0.0f ) {
	    double dist = Num/Den;

	    r.p += dist * r.k;					// Move along the ray the distance to the normal surface.
	} else {
	    return 1;						// BANG!
	}
    }
    if ( i >= ACORN_ITER ) { return 1; }

    return 0;
}

