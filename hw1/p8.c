#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	int fd1[2]; //0 for reading, 1 for writing
	int fd2[2];

	char str[] = "bruh\n";
	char input[100];
	pid_t p;
	pipe(fd1);
	pipe(fd2);
	
	printf("type a phrase: ");
	scanf("%s", input);
	p = fork();

	if (p > 0) //parent
	{
		char output[100];

		close(fd1[0]); 					//close 1 reading
		write(fd1[1], input, strlen(input) + 1);	//write 1
		close(fd1[1]); 					//close 1 write
								
		wait(NULL);
		
		close(fd2[1]);				//close 2 write
		read(fd2[0], output, 100);		//read 2
		printf("parent output: %s\n", output);	
		close(fd2[0]);				//close 2 read
	}
	else
	{
		close(fd1[1]);			//close 1 write
		char output[100];	
		read(fd1[0], output, 100);	//read 1

		printf("child. phrase to be added: %s", str);

		int k = strlen(output);
		for (int i = 0; i < strlen(str); i++)
			output[k++] = str[i];

		close(fd1[0]);
		close(fd2[0]);

		write(fd2[1], output, strlen(output) + 1);
		close(fd2[1]);

		exit(0);
	}


	return 0;
}
