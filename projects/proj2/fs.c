#include "fs.h"

#define NUMBLOCKS (FSSIZE/BLOCKSIZE)
#define NUMFAT NUMBLOCKS
#define ROOT 0
unsigned char* fs; 

typedef struct header
{
	char name[256];
	int fsSize;
	int fatStart;
	int fatSize;
	int rootStart;
}header;

typedef struct block
{
	char data[512];
}block;

typedef struct dirent
{
	char name[256];
	int type;
	int size;
	int firstblock;	
}dirent;

void mapfs(int fd)
{
  if ((fs = mmap(NULL, FSSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
  {
      perror("mmap failed");
      exit(EXIT_FAILURE);
  }
  fprintf(stdout, "mapped fs\n");
}

unsigned short *FAT;

void unmapfs()
{
	munmap(fs, FSSIZE);
}


void formatfs()
{
	memset(fs, 0, FSSIZE);
	struct header *h = (struct header *)fs;
	//h->name[255] = '\0';
	h->fsSize = FSSIZE;
	h->fatStart = BLOCKSIZE;
	h->fatSize = NUMFAT * sizeof(unsigned short);
	h->rootStart = h->fatStart + h->fatSize;

	FAT = (unsigned short *)(fs + h->fatStart);

	for (int i = 0; i < NUMFAT; i++)
	{
		FAT[i] = 0;
	}

	FAT[0] = -1;

	dirent *root = (dirent *)(fs + h->rootStart);

	int maxDirents = BLOCKSIZE / sizeof(dirent);

	for (int i = 0; i < maxDirents; i++)
	{
		root[i].name[0] = '\0';
		root[i].type = 0;
		root[i].size = 0;
		root[i].firstblock = 0;
	}

	fprintf(stdout, "Formatted the new filesystem! FAT[0] points to %d and Header says fat starts at %d!!!\n", FAT[0], h->fatStart);
}


void loadfs()
{
	struct header *h = (struct header*)fs;

	FAT = (unsigned short*)(fs + h->fatStart);

	fprintf(stdout, "Loaded filesystem with FAT starting at %p\n", h->fatStart);
}


void lsfs()
{
	header *h = (header *)fs;
	dirent *root = (dirent *)(fs + h->rootStart);
	int maxDirents = BLOCKSIZE / sizeof(dirent);
	fprintf(stdout, "Listing contents...\n");

	for (int i = 0; i < maxDirents; i++)
	{
		if (root[i].name[0] != '\0')
		{
			fprintf(stdout, "%s \tsize: %d firstblock: %d\n", root[i].name, root[i].size, root[i].firstblock);
		}
	}
}

int fatBlockFinder()
{
	for (int i = 0; i < NUMFAT; i++)
	{
		if (FAT[i] == 0)
		{
			return i;
		}
	}
	return -1;
}

int buildChain(int blocksNeeded)
{
	int first = -2;
	int prev = -2;

	for (int i = 0; i < blocksNeeded; i++)
	{
		int new = fatBlockFinder();
		FAT[new] = USHRT_MAX;

		if (prev != -2)
		{
			FAT[prev] = new;
		}
		if (i == 0)
		{
			first = new;
		}

		prev = new;
	}

	return first;
}

void addfilefs(char* fname)
{
	FILE* fileToAdd = fopen(fname, "r");
	struct stat fstat;
	if (stat(fname, &fstat) != 0)
	{
		perror("stat error");
		exit(1);
	}

	int fileSize = fstat.st_size;
	int blocksNeeded = (fileSize + BLOCKSIZE - 1) / BLOCKSIZE;
	int chain = buildChain(blocksNeeded);

	int current = chain; //index of first data block
	int remain = fileSize;	//# of bytes left

	while (current != USHRT_MAX && remain > 0)
	{
		unsigned char *blockPtr = fs + sizeof(header) + NUMFAT * sizeof(unsigned short) + current * BLOCKSIZE;
		int read = (remain < BLOCKSIZE) ? remain : BLOCKSIZE;

		fread(blockPtr, 1, read, fileToAdd);
		remain -= read;
		current = FAT[current];
	}

	fclose(fileToAdd);

	header *h = (header *)fs;
	dirent *root = (dirent *)(fs + h->rootStart);

	int maxDirents = BLOCKSIZE / sizeof(dirent);
	for (int i = 0; i < maxDirents; i++)
	{
		if (root[i].name[0] == '\0')
		{
			strcpy(root[i].name, fname);
			root[i].type = 1;
			root[i].size = fileSize;
			root[i].firstblock = chain;
			break;
		}
	}


	fprintf(stdout, "Added %s which used %d blocks! first block in chain: %d\n", fname, blocksNeeded, chain);
}


void removefilefs(char* fname)
{

}


void extractfilefs(char* fname)
{

}
