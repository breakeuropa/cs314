#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	printf("p1\n\n");

	int x = 4;

	if (fork())
	{
		x  = 7843;
		printf("parent! %d\n", x);
	}
	else
		printf("child! %d\n", x);

	printf("bruh\n\n");
	return 0;
}
