#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define N 10

sem_t mutex;

void* threadmain(void* args) //we dont know how many arguments are gonna be passed in 
{
	//printf("hi frmo %d\n", (long int) args);
	
	for (int i = 0; i < 10000; i++)
	{
		sem_wait(&mutex);
		//crit section (atomic)
		*((int*)args) += 1; //just increment shared, we are changing shared
		sem_post(&mutex);
	}
}

int main()
{
	pthread_t threads[N];
	int shared = 0;
	sem_init(&mutex, 0, 1);
	for (long int i = 0; i < N; i++)
	{
		pthread_create(&(threads[i]), NULL, threadmain, (void*)&shared); //was (void*) i, only cared about the value of i
	}
	for (int i = 0; i < N; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("%d\n", shared);

	return 0;
}
