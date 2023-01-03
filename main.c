#include <stdlib.h>
#include <math.h>
#include <stdio.h>


int main()
{

    double lambda = 2.0;
    double u = (double)rand() / RAND_MAX;
    double x = -log(1 - u) / lambda;
    printf("Exponential random variable %f\n", x);
    return 1;
}