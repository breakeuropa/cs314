#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
	printf("p2\n\n");

	int file = open("yippee.txt", O_RDWR | O_CREAT);

	if (fork())
	{
		write(file, "yippee from parent!!", strlen("yippee from parent!!"));
		printf("oarent\n");
	}
	else
	{
		write(file, "yippee from cihld!!", strlen("yippee from cihld!!"));
		printf("child\n");
	}
	printf("\n");

	close(file);
	return 0;
}
