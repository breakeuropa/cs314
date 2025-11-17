#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct mystruct
{
	int size;
	char name[256];
}mystruct;

void add (const char* arc, const char* fileName)
{
	FILE *archive = fopen(arc, "ab");
	FILE *toAdd = fopen(fileName, "ab");
	
	if (!archive)
	{
		printf("couldnt open archive\n");
	}
	if (!toAdd)
	{
		printf("couldnt open toAdd\n");
	}
	struct stat fStat;

	if (stat(fileName, &fStat) != 0)
	{
		printf("stat failed in add\n");
		return;
	}
	struct mystruct head;
	strncpy(head.name, fileName, sizeof(head.name) - 1);
	fwrite(&head, sizeof(head), 1, archive);

	fclose(toAdd);
	fclose(archive);
	printf("Added %s\n", head.name);
}

void list (const char* arc)
{
	FILE *archive = fopen(arc, "rb");

	struct mystruct head;

	//printf("b4 while\n");
	while (fread(&head, sizeof(head), 1, archive) == 1)
	{
		printf("%s\n", head.name);
		fseek(archive, head.size, SEEK_CUR);
	}
	//printf("afta\n");
	fclose(archive);
}

void extract (const char* arc, const char* fileName)
{
	FILE *archive = fopen(arc, "rb");

	struct mystruct head;
	int flag = 0;

	while (fread(&head, sizeof(head), 1, archive) == 1)
	{
		if (fileName == NULL || strcmp(head.name, fileName) == 0)
		{
			flag = 1;
			FILE *output = fopen(head.name, "wb");

			char buffer[4096];
			int remain = head.size;

			while (remain > 0)
			{
				int bruh = remain > sizeof(buffer) ? sizeof(buffer) : remain;
				fread(buffer, 1, bruh, archive);
				fwrite(buffer, 1, bruh, output);
				remain -= bruh;
			}
			fclose(output);
			printf("extracted: %s\n", head.name);
			if (fileName) break;
		}
		else
		{
			fseek(archive, head.size, SEEK_CUR);
		}
	}

	fclose(archive);
}

int main(int argc, char *argv[])
{
	const char *op = argv[1];
	const char *arc = argv[2];

	if (strcmp(op, "-a") == 0)
	{
		add(arc, argv[3]);
	}
	else if (strcmp(op, "-l") == 0)
	{
		list(arc);
	}
	else if (strcmp(op, "-e") == 0)
	{
		if (argc == 3)
		{
			extract(arc, NULL);
		}
		else
		{
			extract(arc, argv[3]);
		}
	}
	else
	{
		printf("no\n");
		return 1;
	}

	return 0;
}
