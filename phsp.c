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
pthread_mutex_t chopsticks[MAX_PHSP];
sem_t sems[MAX_PHSP];

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

    num_phsp = atoi(argv[1]);  /* number of philosophers */
    min_think = atoi(argv[2]); /* minimum thinking time for a philosopher */
    max_think = atoi(argv[3]); /* maximum thinking time for a philosopher */
    min_dine = atoi(argv[4]);  /* minimum dining time for a philosopher */
    max_dine = atoi(argv[5]);  /* maximum dining time for a philosopher */
    char *dst = argv[6];
    dst_type = get_dst(dst);
    dine_num = atoi(argv[7]); /* Each philosopher will dine num times */

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