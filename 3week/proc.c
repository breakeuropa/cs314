#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int a = 111;

int main ()
{
	if (fork())
	{
		a = 123;
		printf("parnet: %p %d\n", &a, a);
	}
	else
	{
		if (execv("/bin/ls", NULL))
		{
			perror("execv failed");
		}
		printf("chidlrne\n");
	}

	printf("Hi\n");
	//pause();
	return 0;
}
