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

static unsigned char *blockPtr(int index)
{
	header *h = (header *)fs;
	unsigned char *dataStart = fs + sizeof(header) + NUMFAT * sizeof(unsigned short);
	
	return dataStart + (index * BLOCKSIZE);
}

void unmapfs()
{
	munmap(fs, FSSIZE);
}


void formatfs()
{
	memset(fs, 0, FSSIZE);
	struct header *h = (struct header *)fs;
	h->fsSize = FSSIZE;
	h->fatStart = BLOCKSIZE;
	h->fatSize = NUMFAT * sizeof(unsigned short);
	h->rootStart = 0;

	FAT = (unsigned short *)(fs + h->fatStart);

	for (int i = 0; i < NUMFAT; i++)
	{
		FAT[i] = 0;
	}

	FAT[0] = USHRT_MAX;

	dirent *root = (dirent *)(blockPtr(0));

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

int findDirent(int start, const char *name, int *found, int *entry)
{
	if (start < 0 || start >= NUMFAT) return 0;

	int block = start;
	while (block != USHRT_MAX)
	{
		dirent *entries = (dirent *)blockPtr(block);
		int n = BLOCKSIZE / sizeof(dirent);
		for (int i = 0; i < n; i++)
		{
			if (entries[i].name[0] != '\0' && strcmp(entries[i].name, name) == 0)
			{
				if (found) *found = block;
				if (entry) *entry = i;
				return 1; //found the right dirent
			}
		}

		block = FAT[block];
	}

	return 0;
}

int addDirent(int start, const dirent *new)
{
	int block = start;
	int prev = -1;

	while(1)
	{
		dirent *entries = (dirent *)blockPtr(block);
		int n = BLOCKSIZE / sizeof(dirent);
		
		for (int i = 0; i < n; i++)
		{
			if (entries[i].name[0] == '\0')
			{
				memcpy(&entries[i], new, sizeof(dirent));
				return 0;
			}
		}

		unsigned short next = FAT[block];
		
		if (next == USHRT_MAX)
		{
			int newBlock = fatBlockFinder();
			if (newBlock < 0) return -1;
		
			FAT[newBlock] = USHRT_MAX;
			FAT[block] = newBlock;

			memset(blockPtr(newBlock), 0, BLOCKSIZE);

			dirent *nents = (dirent *)blockPtr(newBlock);
			memcpy(&nents[0], new, sizeof(dirent));
			return 0;
		}
		else
		{
			prev = block;
			block = next;
		}

	}
}

int parentTraverse(const char *path, int *parent, char *name)
{
	if (!path || path[0] == '\0') return -1;

	char temp[strlen(path) + 1]; //copy of the path
	strcpy(temp, path);

	char *p = temp; //pointer to temp, moves past leading 
	if (*p == '/') p++;

	int current = ((header *)fs)->rootStart; 
	char *token = strtok(p, "/"); //first component of path (string, delimiter)
	char *next = strtok(NULL, "/");

	while (next)
	{
		int block, index;
		if(!findDirent(current, token, &block, &index)) 
			return -1;

		dirent *entries = (dirent *)blockPtr(block);
		if (entries[index].type != 2)
			return -1;

		current = entries[index].firstBlock;

		token = next;
		next = strtok(NULL, "/");
	}
	
	strcpy(name, token);
	*parent = current;
	return current;
}

int lookupDirent(const char *path)
{
	header *h = (header *)fs;

	char temp[strlen(path)+1];
	strcpy(temp, path);

	char *p = temp;
	if (*p == '/') p++;

	int current = h->rootStart;

	char *token = strtok(p, "/");

	while(token)
	{
		int block, index;
		if (!findDirent(current, token, &block, &index))
			return -1;

		dirent *entries = (dirent *)blockPtr(block);
		if (entries[index].type != 2)
			return -1;

		current = entries[index].firstBlock;
		token = strtok(NULL, "/");
	}

	return current;
}

int makeDirectory(const char *path)
{
	header *h = (header *)fs;

	char temp[strlen(path) + 1];
	strcpy(temp, path);

	char *p = temp;
	if (*p == '/') p++;

	int current = h->rootStart;

	char *token = strtok(p, "/");
	while(token)
	{
		int block, index;
		if (!findDirent(current, token, &block, &index))
		{
			int newBlock = makeDirectory(
		} 

                dirent *entries = (dirent *)blockPtr(block);
                if (entries[index].type != 2)
                        return -1;

                current = entries[index].firstBlock;
                token = strtok(NULL, "/");
	}
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
	char *path = strrchr(fname, '/'); //find the filename
	if (path)
	{
		path++;
	}
	else
	{
		path = fname;
	}

	header *h = (header *)fs;
	dirent *root = (dirent *)(fs + h->rootStart);

	int maxDirents = BLOCKSIZE / sizeof(dirent);

	int found = -1;
	for (int i = 0; i < maxDirents; i++) //make sure fname and filename match
	{
		if (root[i].name[0] != '\0' && strcmp(root[i].name, fname) == 0)
		{
			found = i;
			break;
		}
	}

	int block = root[found].firstblock;

	while (block != 0 && block != -1 && block != USHRT_MAX)
	{
		unsigned short next = FAT[block];
		FAT[block] = 0;
		block = next;
	}

	root[found].name[0] = '\0';
	root[found].type = 0;
	root[found].size = 0;
	root[found].firstblock = 0;

	fprintf(stdout, "Removed %s\n", fname);

}


void extractfilefs(char* fname)
{

}
