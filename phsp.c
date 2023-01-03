#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>


/* CONSTANTS */
#define MAX_PHSP 27
#define EXPONENTIAL_LITERAL "exponential"
#define UNIFORM_LITERAL "uniform"
#define EXPONENTIAL_DST 1
#define UNIFORM_DST 0
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define MAX_CONDITION 60000 // 60 seconds
#define MIN_CONDITION 1 // 1 ms
#define TRUE 1

/* GLOBAL VARIABLES */
int state[MAX_PHSP]; /* represents the state of philosopher */
int num_phsp;
int max_think;
int min_think;
int max_dine;
int min_dine;
int dst_type;
int num;
sem_t mutex;
sem_t sems[MAX_PHSP];

int get_mean(int min, int max)
{
    return (min + max) / 2;
}

//todo: check the calculation again
double exprand(int min, int max)
{
    int lambda = get_mean(min, max);
    double u = (rand() % (max - min + 1)) + min;
    return -log(1 - u) / lambda;
}


//todo: check the calculation again
int unirand(int min, int max)
{
    return ((rand() / (1 + RAND_MAX)) * ((max - min) + 1)) + min;
}

// get random think or dine time between specified boundaries
// based on distribution type in milliseconds
int get_randtime(int min, int max)
{
    if (dst_type == EXPONENTIAL_DST)
    {
        while (TRUE)
        {
            
            int thinktime = exprand(min, max);
            if (thinktime >= min && thinktime <= max)
            {
                return thinktime;
            }
        }
        
    }
    else if (dst_type == UNIFORM_DST)
    {
        while (TRUE)
        {
            int thinktime = unirand(min, max);
            if (thinktime >= min && thinktime <= max)
            {
                return thinktime;
            }
        }
    }
    
    exit(EXIT_FAILURE);
}

int to_sec(int ms)
{
    return ms / 1000;
}

// get the distribution type as an integer
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


void test(int num_phsp, int phsp_i, int state[], int min_dine, int max_dine)
{
    int left = (phsp_i + 4) % num_phsp;
    int right = (phsp_i + 1) % num_phsp;
    if(state[phsp_i] == HUNGRY && state[left] != EATING && state[right] != EATING)
    {
        state[phsp_i] = EATING;

        int dine_time = get_randtime(min_dine, max_dine);
        int secs = to_sec(dine_time);
        sleep(secs);

        printf("Philosopher %d takes the chopstick %d and %d\n", phsp_i + 1, left + 1, phsp_i + 1);
        printf("Philosopher %d is eating\n", phsp_i + 1);
        sem_post(&mutex);
    }

}

void pickup_chopstick(int num_phsp, int phsp_i)
{
    sem_wait(&mutex);
    state[phsp_i] = HUNGRY;
    printf("Philosopher %d is hungry\n", phsp_i);

    // eat if left and right philosopers are not eating
    test(num_phsp, phsp_i, state, min_dine, max_dine);

    sem_post(&mutex);

    // if no chopticks are available, wait
    sem_wait(&sems[phsp_i]);

    int think_time = get_randtime(min_think, max_think);
    int secs = to_sec(think_time);
    sleep(secs);
    printf("Philosopher %d is dining for %d secs\n", phsp_i + 1, secs);
}

void putdown_chopstick(int num_phsp, int phsp_i)
{   
    
    /*CRITICAL SECTION*/
    sem_wait(&mutex); 
    state[phsp_i] = THINKING;
    printf("Philosopher %d puts down the chopstick %d and %d\n", phsp_i + 1, ( + 4) % num_phsp + 1, phsp_i + 1);
    printf("Philosopher %d is thinking\n", phsp_i + 1);
    /*CRITICAL SECTION*/
    
    test(num_phsp, (phsp_i + 4) % num_phsp, state, min_dine, max_dine); // LEFT
    test(num_phsp, (phsp_i + 1) % num_phsp, state, min_dine, max_dine); // RIGHT

    sem_post(&mutex); // UP Critical Section
}

// philosopher thread
void *philosopher(void *arg)
{
    while (TRUE)
    {
        int* i = arg;

        //think
        int thinktime = get_randtime(min_think, max_think);
        int secs = to_sec(thinktime);
        printf("Philosopher %d is thinking for %d secs\n", *i + 1, secs);
        sleep(secs);

        //pick up chopstick
        pickup_chopstick(num_phsp, *i);

        //eat

        //put down chopstick
        putdown_chopstick(num_phsp, *i);
        
    }
    
}


int main(int argc, int *argv[])
{

    num_phsp = atoi(argv[1]);  /* number of philosophers */
    min_think = atoi(argv[2]); /* minimum thinking time for a philosopher */
    max_think = atoi(argv[3]); /* maximum thinking time for a philosopher */
    min_dine = atoi(argv[4]);  /* minimum dining time for a philosopher */
    max_dine = atoi(argv[5]);  /* maximum dining time for a philosopher */
    char *dst = argv[6];
    dst_type = get_dst(dst);
    num = atoi(argv[7]); /* Each philosopher will dine num times */
    
    // Highest 60 secs, lowest 1 ms
    if (min_think < MIN_CONDITION ||
        max_think > MAX_CONDITION || 
        min_dine < MIN_CONDITION  || 
        max_dine > MAX_CONDITION)
    {
        printf("Invalid high-low time bounds\n");
        exit(EXIT_FAILURE);
    }

    

    printf("******************************\n");
    printf("num_phsp = %d\n", num_phsp);
    printf("min_think = %d\n", min_think);
    printf("max_think = %d\n", max_think);
    printf("min_dine = %d\n", min_dine);
    printf("max_dine = %d\n", max_dine);
    printf("dst = %s\n", dst);
    printf("num = %d\n", num);
    printf("dst type = %d\n", dst_type);
    printf("******************************\n");

    
    int i;
    
    int philosophers[num_phsp]; /* represents a philosopher */
    pthread_t thread_id[num_phsp]; /* represents a philosopher as thread*/
    
    // initialize the mutex
    sem_init(&mutex, 0, 1);
    
    /* 
       initialize the semaphores
       each semaphore represents a chopstick
       so the shared data is the number of chopsticks
    */
    for (i = 0; i < num_phsp; i++)
    {
        sem_init(&sems[i], 0, 0);
        printf("Chopstick %d is available\n", i + 1);
    }

    // initialize philosopher threads
    for (i = 0; i < num_phsp; i++)
    {
        pthread_create(&thread_id[i], NULL, philosopher, &philosophers[i]);
        printf("initialize philosopher %d\n", i + 1);
    }

    // wait for all threads to finish
    for (i = 0; i < num_phsp; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    
    exit(EXIT_SUCCESS);
}