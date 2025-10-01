#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	if(fork() == 0)
	{
		printf("CHild\n");
		close(STDOUT_FILENO);
		printf("Child after close :)\n");
	}
	else
	{
		printf("Parent!!\n");
	}

	return 0;
}
