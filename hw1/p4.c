#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int status;
	pid_t pid = fork();

	if (pid == 0)
	{
		printf("Child!\n");
		
		char* args[] = {"ls", NULL};
		execv("ls", args);
		exit(0);
	}
	else
	{
		printf("Parent!\n");
		waitpid(pid, &status, 0);
	}
	return 0;
}
