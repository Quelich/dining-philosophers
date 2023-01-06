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
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define MAX_TIME_COND 60000 // 60 seconds
#define MIN_TIME_COND 1 // 1 ms

/* GLOBAL VARIABLES */
int state[MAX_PHSP]; /* represents the state of philosopher */
int num_phsp;
int max_think;
int min_think;
int max_dine;
int min_dine;
int dst_type;
int dine_num;
pthread_mutex_t chopsticks[MAX_PHSP]; // binary mutex for each chopstick
sem_t sems[MAX_PHSP];

double get_mean(int min, int max)
{
    return (min + max) / 2;
}

int exprand(int min, int max)
{
    int lambda = get_mean(min, max);
    int uni = unirand(min, max);
    double u = (uni % (max - min + 1)) + min; 
    return -log(1 - u) / lambda;
}

int unirand(int min, int max)
{
    int range = max - min + 1;
    int secure_max = RAND_MAX - (RAND_MAX % range);
    int rnd;
    do
    {
        rnd = rand();
    } while (rnd >= secure_max);
    return min + (rnd % range);
}

int to_ns(int ms)
{      
    return ms * 1000000;
}


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

int get_thinktime()
{
    if (dst_type == EXPONENTIAL_DST)
    {
       
    }
    else if (dst_type == UNIFORM_DST)
    {
        return unirand(min_think, max_think);
    }
}

void think()
{
    sleep(2);
}

void eat()
{
    sleep(2);
}

// philosopher thread
void *philosopher(void *arg)
{
    int  i = *((int *)arg);
    int j = 0;
    int left = i;
    int right = (i + 1) % num_phsp;
    while (j < num_phsp)
    {
        i = *((int *)arg);
        // think
        printf("Philosopher %d is Thinking\n", i);
        think();
        printf("Philosopher %d is Hungry\n", i);
        pthread_mutex_lock(&chopsticks[left]);
        pthread_mutex_lock(&chopsticks[right]);
        // eat
        printf("Philosopher %d is Eating\n", i);
        eat(); 
        printf("Philosopher %d is Done Eating\n", i);
        pthread_mutex_unlock(&chopsticks[left]);
        pthread_mutex_unlock(&chopsticks[right]);
        j++;
    }
    pthread_exit(NULL);
}


int main(int argc, int *argv[])
{
    srand(time(NULL));

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
    printf("num = %d\n", dine_num);
    printf("dst type = %d\n", dst_type);
    printf("******************************\n");
    
    
    int unitime_rand = unirand(min_think, max_think);
    int exptime_rand = exprand(min_think, max_think);
    printf("uniform rand = %d ms,\n", unitime_rand);
    printf("exponential rand = %d ms,\n", exptime_rand);
    
    
    // int i;
    // int philosophers[num_phsp]; 
    // pthread_t tids[num_phsp]; /* philosopher threads */
    
    // // initialize the semaphores for each chopstick
    // for (i = 0; i < num_phsp; i++)
    // {
    //     pthread_mutex_init(&chopsticks[i], NULL);
    // }

    // // initialize the philosophers
    // for (i = 0; i < num_phsp; i++)
    // {
    //     philosophers[i] = i;
    // }

    // // initialize philosopher threads
    // for (i = 0; i < num_phsp; i++)
    // {
    //     pthread_create(&tids[i], NULL, philosopher, &philosophers[i]);
    //     printf("initialize philosopher %d\n", i + 1);
    // }

    // // wait for all threads to finish
    // for (i = 0; i < num_phsp; i++)
    // {
    //     pthread_join(tids[i], NULL);
    // }
    
    exit(EXIT_SUCCESS);
}