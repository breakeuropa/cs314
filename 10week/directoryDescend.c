#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	struct dirent *pDirent;
	DIR *pDir;
	struct stat fStat;


	if (argc != 2) //wrong arguenmtns
	{
		printf("no\n");
		return 1;
	}
	
	printf("openibg thing\n");
	pDir = opendir(argv[1]); //open what i tell it to
	printf("arg1 is %s\n", argv[1]);

	if (pDir == NULL)
	{
		printf("bithc is null\n");
		return 1;
	}
	
	printf("while\n");	
	while ((pDirent = readdir(pDir)) != NULL)
	{
		printf("[%s]\t\t\t\tsize: %ld\n", pDirent->d_name, fStat.st_size);
	}

	closedir(pDir);

	return 0;
}
