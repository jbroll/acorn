
#include <stdio.h>
#include <math.h>

int main()
{
    int i;

    for ( i = 1; i <=25; i++ ) {
	printf("%.17f\n", sqrt((double)i));
    }
}

