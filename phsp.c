#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

/* CONSTANTS */
#define MAX_PHSP 27
#define EXPONENTIAL_LITERAL "exponential"
#define UNIFORM_LITERAL "uniform"
#define EXPONENTIAL_DST 1
#define UNIFORM_DST 0
#define MAX_TIME_COND 60000 // 60 seconds
#define MIN_TIME_COND 1 // 1 ms

/* GLOBAL VARIABLES */
int num_phsp;
int max_think;
int min_think;
int max_dine;
int min_dine;
int dst_type;
int dine_num;
pthread_mutex_t chopsticks[MAX_PHSP]; // binary mutex for each chopstick
sem_t sems[MAX_PHSP];

// Calculate mean according to given min and max
double get_mean(int min, int max)
{
    return (min + max) / 2;
}

// Generate a random number between 0 and 1
double rand_01()
{
    srand(time(NULL)); 
    return (double)rand() / RAND_MAX;
}

// Generate a random number between min and max based on exponential distribution
double exprand(int min, int max)
{   
    
    double lambda = 1 / get_mean(min, max);
    double exp_value;
    double u = rand_01();
    exp_value = -log(1 - u) / lambda;

    while (exp_value < min || exp_value > max)
    {
        u = rand_01();
        exp_value = -log(1 - u) / lambda;
        //printf("exp_value = %f\n", exp_value);
    }
    
    //printf("lambda = %f\n", lambda);
    //printf("u = %f\n", u);
    //printf("exp_value = %f\n", ceil(exp_value));
    return ceil(exp_value);
}

// Generate a random number between min and max based on uniform distribution
int unirand(int min, int max)
{
    srand(time(NULL));  
    int range = max - min + 1;
    int secure_max = RAND_MAX - (RAND_MAX % range);
    int rnd;
    do
    {
        rnd = rand();
    } while (rnd >= secure_max);
    //printf("uni_rand = %d\n", min + (rnd % range));
    return min + (rnd % range);
}

// Convert milliseconds to microseconds
int to_micro(int ms)
{      
    return ms * 1000;
}


// Get distribution type from command line argument
int get_dst(char *dst)
{
    if (strcmp(dst, EXPONENTIAL_LITERAL) == 0)
    {
        return EXPONENTIAL_DST;
    }
    else if (strcmp(dst, UNIFORM_LITERAL) == 0)
    {
        return UNIFORM_DST;
    }
    printf("Invalid distribution type\n");
    exit(EXIT_FAILURE);
}

// Calculate think time based on distribution type
int get_thinktime()
{
    if (dst_type == EXPONENTIAL_DST)
    {
       return (int)exprand(min_think, max_think);
    }
    else if (dst_type == UNIFORM_DST)
    {
        return unirand(min_think, max_think);
    }
    printf("Unable to get think time\n");
    exit(EXIT_FAILURE);
}

// Calculate dine time based on distribution type
int get_dinetime()
{
    if (dst_type == EXPONENTIAL_DST)
    {
       return (int)exprand(min_dine, max_dine);
    }
    else if (dst_type == UNIFORM_DST)
    {
        return unirand(min_dine, max_dine);
    }
    printf("Unable to get dine time\n");
    exit(EXIT_FAILURE);
}

// Demonstrate thinking 
void think(int ms)
{
    int mic = to_micro(ms);
    usleep(mic);
}

// Demonstrate eating
void eat(int ms)
{
    int mic = to_micro(ms);
    usleep(mic);
}

// philosopher thread
void *philosopher(void *arg)
{
    int  i = *((int *)arg);
    int j = 0;
    int k;
    int left = i;
    int right = (i + 1) % num_phsp;
    while (j < num_phsp)
    {   
        /*HUNGRY TIME*/
        i = *((int *)arg);
        // THINK
        int thinktime = get_thinktime();
        think(thinktime);
        printf("Philosopher %d is Hungry\n", i);
        pthread_mutex_lock(&chopsticks[left]);
        pthread_mutex_lock(&chopsticks[right]);
        /*HUNGRY TIME*/
        // EAT
        printf("Philosopher %d is Eating\n", i);
        int dinetime = get_dinetime();       
        eat(dinetime); 
        printf("Philosopher %d is Done Eating\n", i);
        pthread_mutex_unlock(&chopsticks[left]);
        pthread_mutex_unlock(&chopsticks[right]);
        j++;
    }
    pthread_exit(NULL);
}


int main(int argc, int *argv[])
{
    num_phsp = atoi(argv[1]);  
    min_think = atoi(argv[2]); 
    max_think = atoi(argv[3]); 
    min_dine = atoi(argv[4]);  
    max_dine = atoi(argv[5]);  
    char *dst = argv[6];
    dst_type = get_dst(dst);
    dine_num = atoi(argv[7]); 

    // Highest 60 secs, lowest 1 ms
    if (min_think < MIN_TIME_COND ||
        max_think > MAX_TIME_COND || 
        min_dine < MIN_TIME_COND  || 
        max_dine > MAX_TIME_COND)
    {
        printf("Invalid high-low time bounds\n");
        exit(EXIT_FAILURE);
    }

    // DEBUG
    printf("******************************\n");
    printf("num_phsp = %d\n", num_phsp);
    printf("min_think = %d ms\n", min_think);
    printf("max_think = %d ms\n", max_think);
    printf("min_dine = %d ms\n", min_dine);
    printf("max_dine = %d ms\n", max_dine);
    printf("dst = %s\n", dst);
    printf("dine num = %d\n", dine_num);
    printf("dst type = %d\n", dst_type);
    printf("******************************\n");
    
    

    int i;
    int philosophers[num_phsp]; 
    pthread_t tids[num_phsp]; /* philosopher threads */
    
    // initialize the semaphores for each chopstick
    for (i = 0; i < num_phsp; i++)
    {
        pthread_mutex_init(&chopsticks[i], NULL);
    }

    // initialize the philosophers
    for (i = 0; i < num_phsp; i++)
    {
        philosophers[i] = i;
    }

    // initialize philosopher threads
    for (i = 0; i < num_phsp; i++)
    {
        pthread_create(&tids[i], NULL, philosopher, &philosophers[i]);
        printf("initialize philosopher %d\n", i + 1);
    }

    // wait for all threads to finish
    for (i = 0; i < num_phsp; i++)
    {
        pthread_join(tids[i], NULL);
    }
    
    exit(EXIT_SUCCESS);
}