#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#define P 5
#define C 5
#define BUFSIZE 5

char buffer[BUFSIZE] = {0};
char toProduce[] = "hi there";
int pIndex = 0;
int cIndex = 0;
sem_t mutex;
sem_t empty, full; 								//empty = how many slots are empty, full = how many slots are full

void* producer(void* args) 							//should produce when there is space in buffer to take stuff in
{
	//for (int i = 0; i < strlen(toProduce); i++)
	for (int i = 0; i < 10; i++)
	{
		sem_wait(&empty); 						//wait while theres still empty
		sem_wait(&mutex);
		//buffer[pIndex%BUFSIZE] = toProduce[i];                        //mod BUFSIZE keeps range between 0 and 4
		buffer[pIndex%BUFSIZE] = 'a'+(int)args; 			//mod BUFSIZE keeps range between 0 and 4
		pIndex++;
		sem_post(&mutex);
		sem_post(&full); 						//incremnet full slots
	}
}

void* consumer(void* args) 
{
	while(1)
	{
		sem_wait(&full); 						//waits for all slots to be full
		sem_wait(&mutex);
                fprintf(stderr, "%c", buffer[cIndex%BUFSIZE]); 
		cIndex++;
		totalConsumed++
		sem_post(&mutex);
		sem_post(&empty);						//increment empty cuase i took stuff off the buffer
	}
}

int main()
{
	pthread_t threads[P+C];
	sem_init(&mutex, 0 ,1);
	sem_init(&empty, 0, BUFSIZE);						//empty is the same size as the array
	sem_init(&full, 0, 0);							//starts at 0 cause its empty

	for (long int i = 0; i < P; i++)
	{
		pthread_create(&(threads[i]), NULL, producer, (void*) i);
	}
	for (long int i = 0; i < C; i++)                                                                                                                            {
                pthread_create(&(threads[i]), NULL, consumer, NULL);
        }
	for (int i = 0; i < P+C; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}
