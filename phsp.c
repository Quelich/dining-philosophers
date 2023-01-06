#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>


/* CONSTANTS */
#define MAX_PHSP 27
#define EXPONENTIAL_LITERAL "exponential"
#define UNIFORM_LITERAL "uniform"
#define EXPONENTIAL_DST 1
#define UNIFORM_DST 0
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define MAX_CONDITION 60000 // 60 seconds
#define MIN_CONDITION 1 // 1 ms


/* GLOBAL VARIABLES */
int state[MAX_PHSP]; /* represents the state of philosopher */
int num_phsp;
int max_think;
int min_think;
int max_dine;
int min_dine;
int dst_type;
int dine_num;
pthread_mutex_t mutex;
sem_t sems[MAX_PHSP];

// int get_mean(int min, int max)
// {
//     return (min + max) / 2;
// }

// //todo: check the calculation again
// double exprand(int min, int max)
// {
//     int lambda = get_mean(min, max);
//     double u = (rand() % (max - min + 1)) + min;
//     return -log(1 - u) / lambda;
// }


// //todo: check the calculation again
// double unirand(int min, int max)
// {
//     return ((rand() / (1 + RAND_MAX)) * ((max - min) + 1)) + min;
// }

// get random think or dine time between specified boundaries
// based on distribution type in milliseconds
// int get_randtime(int min, int max)
// {   
//     int flag = 0;
//     if (dst_type == EXPONENTIAL_DST)
//     {   
        
//         while (flag != 1)
//         {
            
//             int thinktime = (int)exprand(min, max);
//             if (thinktime >= min && thinktime <= max)
//             {   
//                 flag = 1;
//                 return thinktime;
//             }
//         }
        
//     }
//     else if (dst_type == UNIFORM_DST)
//     {
//         while (flag != 1)
//         {
//             int thinktime = unirand(min, max);
//             if (thinktime >= min && thinktime <= max)
//             {
//                 return thinktime;
//             }
//         }
//     }
    
//     exit(EXIT_FAILURE);
// }

// double to_sec(int ms)
// {
//     return ms / 1000;
// }

// // get the distribution type as an integer
// int get_dst(char *dst)
// {
//     if (strcmp(dst, EXPONENTIAL_LITERAL) == 0)
//     {
//         return EXPONENTIAL_DST;
//     }
//     else if (strcmp(dst, UNIFORM_LITERAL) == 0)
//     {
//         return UNIFORM_DST;
//     }
//     printf("Invalid distribution type\n");
//     exit(EXIT_FAILURE);
// }

void test(int i)
{   
    int left = (i + num_phsp - 1) % num_phsp;
    int right = (i + 1) % num_phsp;
    if (state[i] == HUNGRY && state[left] != EATING && state[right] != EATING)
    {
        state[i] = EATING;
        printf("Philosopher %d is picking up chopsticks %d and %d", i, left, i);
        sem_post(&sems[i]);
    }
}

void pickup(int i)
{
    pthread_mutex_lock(&mutex);
    state[i] = HUNGRY;
    test(i);
    pthread_mutex_unlock(&mutex);
    sem_wait(&sems[i]);
}

void putdown(int i)
{
    int left = (i + num_phsp - 1) % num_phsp;
    int right = (i + 1) % num_phsp;
    pthread_mutex_lock(&mutex);
    state[i] = THINKING;
    test(left);
    test(right);
    pthread_mutex_unlock(&mutex);

}

// philosopher thread
void *philosopher(void *arg)
{
    int i;
    int j = 0;
    while (j < num_phsp)
    {
        i = *((int *)arg);
        // think
        printf("Philosopher %d is Thinking\n", i);
        sleep(2); //TODO: change to get_randtime
        // pickup chopstick[i] chopstick[(i + 1) % num_phsp]
        printf("Philosopher %d is Hungry\n", i);
        pickup(i);
        // eat
        printf("Philosopher %d is Eating\n", i);
        sleep(1); //TODO: change to get_randtime
        printf("Philosopher %d is Done Eating\n", i);
        // putdown chopstick[i] chopstick[(i + 1) % num_phsp]
        putdown(i);
        j++;
    }
}

 
int main(int argc, int *argv[])
{

    num_phsp = MAX_PHSP;  /* number of philosophers */
    // min_think = atoi(argv[2]); /* minimum thinking time for a philosopher */
    // max_think = atoi(argv[3]); /* maximum thinking time for a philosopher */
    // min_dine = atoi(argv[4]);  /* minimum dining time for a philosopher */
    // max_dine = atoi(argv[5]);  /* maximum dining time for a philosopher */
    // char *dst = argv[6];
    // dst_type = get_dst(dst);
    // dine_num = atoi(argv[7]); /* Each philosopher will dine num times */
    
    // Highest 60 secs, lowest 1 ms
    // if (min_think < MIN_CONDITION ||
    //     max_think > MAX_CONDITION || 
    //     min_dine < MIN_CONDITION  || 
    //     max_dine > MAX_CONDITION)
    // {
    //     printf("Invalid high-low time bounds\n");
    //     exit(EXIT_FAILURE);
    // }

    // DEBUG
    printf("******************************\n");
    printf("num_phsp = %d\n", num_phsp);
    // printf("min_think = %d ms\n", min_think);
    // printf("max_think = %d ms\n", max_think);
    // printf("min_dine = %d ms\n", min_dine);
    // printf("max_dine = %d ms\n", max_dine);
    // printf("dst = %s\n", dst);
    // printf("num = %d\n", dine_num);
    // printf("dst type = %d\n", dst_type);
    printf("******************************\n");

    
    int i;
    int philosophers[num_phsp]; /* represents a philosopher */
    pthread_t tids[num_phsp]; /* represents a philosopher as thread*/
    
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