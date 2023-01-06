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
        // pickup(left);
        // pickup(right);
        pthread_mutex_lock(&chopsticks[left]);
        pthread_mutex_lock(&chopsticks[right]);
        // eat
        printf("Philosopher %d is Eating\n", i);
        eat(); 
        printf("Philosopher %d is Done Eating\n", i);
        // putdown(left);
        // putdown(right);
        pthread_mutex_unlock(&chopsticks[left]);
        pthread_mutex_unlock(&chopsticks[right]);
        j++;
    }
    pthread_exit(NULL);
}

 
int main(int argc, int *argv[])
{

    num_phsp = MAX_PHSP;  /* number of philosophers */
    

    // DEBUG
    printf("******************************\n");
    printf("num_phsp = %d\n", num_phsp);
    printf("******************************\n");
    
    int i;
    int philosophers[num_phsp]; 
    pthread_t tids[num_phsp]; /* philosopher threads */
    
    // initialize the semaphores
    sem_init(&chopsticks, 0, 1);
    
    /* 
       initialize the semaphores
       each semaphore represents a chopstick
       so the shared data is the number of chopsticks
    */
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