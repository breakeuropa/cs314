#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	printf("p5\n\n");
	int w;
        pid_t p = fork();
        if (p == 0)
        {
		w = wait(NULL);
                printf("Child. Wait: %d\n\n", w);
		exit(0);
        }
        else
        {
                printf("Parent\n");
        }
	printf("\n");	
	return 0;
}
