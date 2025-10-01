#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	printf("p3\n\n");
	pid_t p = fork();
	if (p == 0)
	{
		printf("Hello\n");
	}
	else
	{
		wait(NULL);
		printf("Bye bye\n\n");
	}

	return 0;
}
