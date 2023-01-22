/*
    METADATA
    - time unit: millisecond (ms)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

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
int dine_count[MAX_PHSP];
double hungry_times[MAX_PHSP];

/* SYNCHRONOZATION */
pthread_mutex_t chopsticks[MAX_PHSP];

double get_avg_hungrytime()
{   
    int i;
    double sum, avg;
    for (i = 0; i < num_phsp; i++)
    {
        sum += hungry_times[i];
    }

    avg = sum / num_phsp;
    return avg;
}

double get_std_deviation_hungrytime()
{    
    int i;
    double std_dev;
    double avg = get_avg_hungrytime();

    for (i = 0; i < num_phsp; i++)
    {
        std_dev += pow(hungry_times[i] - avg, 2);
    }

    std_dev = sqrt(std_dev / (num_phsp - 1));
    return std_dev;
}

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
    }
    return round(exp_value);
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

// Demonstrate thinking by simply waiting for awhile
void think(int ms)
{
    int mic = to_micro(ms);
    usleep(mic);
}

// Demonstrate eating by simply waiting for awhile
void eat(int ms)
{
    int mic = to_micro(ms);
    usleep(mic);
}

void wait(int l, int r)
{
    pthread_mutex_lock(&chopsticks[l]);
    pthread_mutex_lock(&chopsticks[r]);
}

void signal(int l, int r)
{
    pthread_mutex_unlock(&chopsticks[l]);
    pthread_mutex_unlock(&chopsticks[r]);
}

// philosopher thread
void *philosopher(void *arg)
{
    int  i = *((int *)arg);
    int k;
    int left = i;
    int right = (i + 1) % num_phsp;
    while (dine_count[i] < dine_num)
    {   
        struct timespec begin, end;             /*START TICKING UNGRY TIME*/
        clock_gettime(CLOCK_REALTIME, &begin);
        i = *((int *)arg);
        // THINK
        int thinktime = get_thinktime();
        think(thinktime);
        wait(left, right);
        clock_gettime(CLOCK_REALTIME, &end);    /*STOP TICKING HUNGRY TIME*/
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed_ms= (seconds + nanoseconds*1e-9) * 1000;
        printf("Philosopher %d is Hungry for %f ms\n", i, elapsed_ms);
        hungry_times[i] += elapsed_ms;
        // EAT
        printf("Philosopher %d is Eating\n", i);
        int dinetime = get_dinetime();       
        eat(dinetime); 
        printf("Philosopher %d is Done Eating\n", i);
        dine_count[i]++;
        signal(left, right);
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

    // Print results
    for ( i = 0; i < num_phsp; i++)
    {
        printf("Hungry time for philosopher %d: %f\n", i, hungry_times[i]);
    }
    
    double avg = get_avg_hungrytime();
    double std_dev = get_std_deviation_hungrytime();
    printf("Average hungry time: %f ms\n", avg);
    printf("Standard deviation of hungry time: %f ms\n", std_dev);

    for ( i = 0; i < num_phsp; i++)
    {
        printf("Dine count for philosopher %d: %d\n", i, dine_count[i]);
    }
    
    exit(EXIT_SUCCESS);
}