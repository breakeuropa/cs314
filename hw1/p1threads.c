#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* changer (void* args)
{
	*((int*)args) += 1;
	printf("chanign args\n\n");
}

int main()
{
	printf("threads p1\n\n");
	
	pthread_t threads[2];

	int x = 4;
	printf("x: %d\n\n", x);

	for (long int i = 0; i < 2; i++)
	{
		pthread_create(&(threads[i]), NULL, changer, (void*)&x);
	}
	for (int i = 0; i < 2; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	printf("x: %d\n\n", x);
	return 0;
}
