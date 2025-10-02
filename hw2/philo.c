#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#define N 5
#define LEFT (i + N - 1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
sem_t mutex; 
sem_t s[N];

void test(long int args)
{
	//printf("\ntest\n");

	if (state[(int)args] == HUNGRY 
		&& state[(((int)args + N - 1) % N)] != EATING
		&& state[(((int)args + 1) % N)] != EATING)
	{
		state[(int)args] = EATING;
		sem_post(&s[(int)args]);
	}
}

void put_forks(long int args)
{
	sem_wait(&mutex);

	state[(int)args] = THINKING;
	printf("philosopher %d has put down forks\n", args);
	test((args + N - 1) % N); //LEFT
	test((args + 1) % N); //RIGHT
	
	sem_post(&mutex);
}

void take_forks(long int args)
{
	sem_wait(&mutex);

	state[(int)args] = HUNGRY;
	printf("philosopher %d has taken forks\n", args);
	test(args);

	sem_post(&mutex);
	sem_wait(&s[(int)args]);
}

void think(long int args)
{
	 printf("philosopher %d is thinking...\n", args);
}

void eat(long int args)
{
	if (state[(long int)args] == EATING)
	{
		 printf("\nphilosopher %d is eating...\n", args);
	}
}


void* philosopher(void* args)
{
	printf("\n[philosopher %d has arrived]\n", (long int) args);
	
	think((long int) args);
	take_forks((long int) args);
	eat((long int) args);
	put_forks((long int) args);
}

int main()
{
	pthread_t threads[N]; //makes an array of 5 threads
	sem_init(&mutex, 0, 1);
	sem_init(&s[N], 0, N);

	for (long int i = 0; i < N; i++)
	{
		pthread_create(&(threads[i]), NULL, philosopher, (void*) i); 
	}
	for (int i = 0; i < N; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("\n");
	return 0;
}
