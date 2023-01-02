#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPONENTIAL_DST "exponential"
#define UNIFORM_DST "uniform"

int main(int argc, int *argv[])
{
   

    int num_phsp = atoi(argv[1]); /* number of philosophers */
    int min_think = atoi(argv[2]); /* minimum thinking time for a philosopher */
    int max_think = atoi(argv[3]); /* maximum thinking time for a philosopher */ 
    int min_dine = atoi(argv[4]); /* minimum dining time for a philosopher */
    int max_dine = atoi(argv[5]); /* maximum dining time for a philosopher */
    char *dst = argv[6];
    int num = atoi(argv[7]); /* Each philosopher will dine num times */

    printf("num_phsp = %d\n", num_phsp);
    printf("min_think = %d\n", min_think);
    printf("max_think = %d\n", max_think);
    printf("min_dine = %d\n", min_dine);
    printf("max_dine = %d\n", max_dine);
    printf("dst = %s\n", dst);
    printf("num = %d\n", num);

    

    exit(EXIT_SUCCESS);
}