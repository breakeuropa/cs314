#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/mman.h>

#define ADD 1
#define LIST 2

typedef struct header
{
	char name[256];
	int size;
} header;

int main(int argc, char* argv[])
{
	char* inFile;	
	int flags = 0;
	char opt;

	FILE *archive = fopen(argv[1], "a+");
	
	while ((opt = getopt(argc, argv, "a:l")) != -1)
	{
		switch (opt)
		{
			case 'l':
				flags = flags | LIST;
				break;
			case 'a':
				flags = flags | ADD;
				inFile = (optarg);
				break;
			default: 
				printf("no\n");
				exit(0);
		}
	}

	if (flags &LIST)
	{
		printf("list flag is on\n");
	}
	if (flags &ADD)
	{
		printf("add flag is on with %s\n", inFile);
	}

	if (flags &ADD) 
	{
		header* newHeader = malloc(sizeof(header));
		FILE* input = fopen(inFile, "r");
		strcpy(newHeader->name, inFile);
		
		fseek(input, 0, SEEK_END); //file stream @ very end
		newHeader->size = ftell(input); //should store the size of the file 
		fseek(input, 0, SEEK_SET); //resets back to the beginning

		char buffer;
		fwrite(newHeader, 1, sizeof(header), archive);
		
		while (fread(&buffer, 1, 1, input) != 0)
		{
			fwrite(&buffer, 1, 1, archive); //copy?
		}
	}
	if (flags &LIST)
	{
		/*
		header myHeader;

		fseek(archive, 0, SEEK_SET); //sets it back to the beginning of the file
		
		while (fread(&myHeader, 1, sizeof(header), archive) != 0)
		{
			printf("%s %d\n", myHeader.name, myHeader.size);
			fseek(archive, myHeader.size, SEEK_CUR);
		}
		*/

		fseek(archive, 0, SEEK_END);
		char* archiveMap = mmap(NULL, ftell(archive), PROT_READ, MAP_PRIVATE, fileno(archive), 0);//NULL location, length, protections,.,.,offset 

		header* myHeader = (header*)archiveMap;
		printf("%s %d\n", myHeader->name, myHeader->size);
		
		//archiveMap is the first header, the first byte of the file
		int dataSize = myHeader->size;
		myHeader = (header*)(archiveMap + sizeof(header) + dataSize); //char pointer archiveMap adding 1 to it adds to the address
									      //size is in bytes, if add 1 to header pointer, 
									      //i get address plus 1 * size of header
									      //archiveMap is char pointer, add sizeof header to addy
									      //move into memory to the next place
									      //define blocks as array of 512 size idk

		printf("%s %d\n", myHeader->name, myHeader->size);
	}


	return 0;
}
