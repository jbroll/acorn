#include <stdlib.h>
#include <stdio.h>

/* From Hedser van Brug SPIE paper 
   Efficient Cartesian representation of Zernike polynomials in computer memory 
   spie.org/etop/1997/382_1.pdf
*/

int Fact(int n) {
    int f = 1, i;
    for (i=1;i<=n;i++) f*=i;
    return f;
}

int Binom(n,k) {
    return Fact(n) / Fact(k) / Fact(n-k);
}

CalcZern(int n, int m, double **f, double scaling) {
    int p, q, l, xpow, ypow;
    float factor;
    int i,j,k;

    l=n-2*m;
    if (l <= 0) {
	p=0;
	q=(n%2==0 ? -l/2 : (-l-1)/2 );
    } else {
	p=1;
	q= (n%2==0 ? l/2 -1: (l-1)/2);
    }
    l = (l<0 ? -l : l) ;   
    m = (n-l)/2;
    
    for (i=0; i<=q; i++) {
	for (j=0; j<=m;j++) {
	    for (k=0; k<=(m-j); k++) {
		factor = ((i+j)%2==0 ? 1 : -1);
		factor *= Binom( l, 2*i+p);
		factor *= Binom(m-j, k);
		factor *= Fact(n-j) / ( Fact(j) * Fact(m-j) * Fact(n-m-j));
		ypow = 2 * (i+k) + p;
		xpow = n - 2 * (i+j+k) - p;
		f[xpow][ypow] += factor*scaling;
	    }
	}
    }
}


#define MAXZERN 100

printformula(double ***y, int i, int n, int m)
{
    int j,k;
    double norm;

    int terms = 0;

    norm=2*(n+1)/(1+(m==0));

    printf("sqrt(%g) * (", norm );

    for (j=0; j<=n; j++) {
	for (k=0; k<=n; k++) {
	    if ( y[i][j][k] ) {
		if ( terms ) { printf(" + "); }
		terms++;

		printf("%g",y[i][j][k]);
		if (j) {
		    if (j==1) 
			printf("*x"); 
		    else  
			printf("*x^%d", j);
		}
		if (k) {
		    if (k==1) 
			printf("*y"); 
		    else  
			printf("*y^%d", k);
		}
	    }
	}
    }
    if ( !terms ) { printf("0 "); }

    printf(")");
}


main(int argc, char **argv) {


    int nzern = atoi(argv[1]);
    double ***z, ***x, ***y;
    int n,m,i,j,k,l;

    z = calloc(MAXZERN, sizeof *z);
    x = calloc(MAXZERN, sizeof(*z));
    y = calloc(MAXZERN, sizeof(*z));

    printf("n\tm\tz\tdx\tdy\n");
    printf("-\t-\t-\t----\t----\n");


    for (n=0, i=1; i<=nzern; n++) {
	for (m=n; m>=0; m--) {
	    z[i] = calloc(n+1, sizeof(**z));
	    x[i] = calloc(n+1, sizeof(**z));
	    y[i] = calloc(n+1, sizeof(**z));
	    for (j=0; j<n+1; j++) {
		z[i][j] = calloc(n+1,sizeof (***z));
		x[i][j] = calloc(n+1,sizeof (***z));
		y[i][j] = calloc(n+1,sizeof (***z));
	    }
	    CalcZern(n, m, z[i], 1.0);
	    
	    for (j=0; j<n; j++) {
		for (k=0; k<=n; k++) {
		    x[i][j][k] = z[i][j+1][k] * (j+1);
		}
	    }

	    for (j=0; j<=n; j++) {
		for (k=0; k<n; k++) {
		    y[i][j][k] = z[i][j][k+1] * (k+1);
		}
	    }

	    l = n - 2 * m;  /* Convert to Noll n,m convention */
	    printf("%d\t%d\t", n, l);
	    printformula(z,i,n,l);
	    printf("\t");
	    printformula(x,i,n,l);
	    printf("\t");
	    printformula(y,i,n,l);
	    printf("\n");
	    i++;
	}
    }
}

