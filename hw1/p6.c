#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	printf("p6\n\n");
	int w;
	int status;
        pid_t p = fork();
        pid_t d = fork();
	
	if (waitpid(d, &status, WNOHANG) == 0)
	{
		printf("Child D is still running\n");
	}
	if (p == 0)
        {
		w = wait(NULL);
                printf("Child P. Wait: %d\n\n", w);
		exit(0);
        }
        else
        {
                printf("Parent P\n");
        }	
	return 0;
}
