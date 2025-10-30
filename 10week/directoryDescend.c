#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAXBINS 10000

void directoryDescend(const char *dirname, int binWidth, int *bin)
{
	
	struct dirent *pDirent;
	DIR *pDir;
	struct stat fStat;

	char path[4096];
	
	//printf("opening thing\n");

	pDir = opendir(dirname); //open what i tell it to

	//printf("arg1 is %s\n", argv[1]);

	if (pDir == NULL)
	{
		printf("is null\n");
		return;
	}
	
	while ((pDirent = readdir(pDir)) != NULL)
	{
		if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
		{
			continue;
		}	
		snprintf(path, sizeof(path), "%s/%s", dirname, pDirent->d_name);

		if (stat(path, &fStat) == -1)
		{
			printf("stat failed\n");
			return;
		}
		
		printf("size: %ld\tname: [%s]\n", fStat.st_size, pDirent->d_name);
		
		if (S_ISREG(fStat.st_mode))
		{
			int binIndex = fStat.st_size / binWidth;
			if (binIndex < MAXBINS)
			{
				bin[binIndex]++;
			}
		}
		else if (S_ISDIR(fStat.st_mode))
		{
			printf("\nDirectory Contents:\n");	
			directoryDescend(path, binWidth, bin);
		}
	
		//printf("size: %ld\tname: [%s]\n", fStat.st_size, pDirent->d_name);
	}

	closedir(pDir);
}

int main(int argc, char *argv[])
{
	int bin[MAXBINS] = {0};
	int binWidth = atoi(argv[2]);

	if (argc != 3) //wrong arguenmtns
	{
		printf("no\n");
		return 1;
	}
	
	directoryDescend(argv[1], binWidth, bin);

	printf("\nHistogram:\n");
	for (int i = 0; i < MAXBINS; i++)
	{
		if (bin[i] > 0)
		{
			printf("\tbin: %d-%d %d files\n", (i * binWidth), ((i + 1) * binWidth - 1), bin[i]);
		}
	}

	return 0;
}
