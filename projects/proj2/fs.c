#include "fs.h"

#define NUMBLOCKS (FSSIZE/BLOCKSIZE)
#define NUMFAT NUMBLOCKS
#define ROOT 0
#define TYPEFILE 1
#define TYPEDIR 2
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
	h->fatStart = sizeof(header);
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
	//int prev = -1;

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
			//prev = block;
			block = next;
		}

	}
}
/*
int pathTraverse(const char *path, int *parent, char *name)
{
	if (!path || path[0] == '\0') return -1;

	char temp[strlen(path) + 1]; //copy of the path
	strcpy(temp, path);

	char *p = temp; //pointer to temp, moves past leading / 
	if (*p == '/') p++;

	header *h = (header *)fs;
	int current = h->rootStart; 
	char *token = strtok(p, "/"); //first component of path (string, delimiter)
	if (!token) return -1;
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
	name[255] = '\0';
	*parent = current;
	return current;
}
*/

int pathTraverse(const char *path, char *child, int *parent)
{
	if (!path || path[0] != '/') return -1;

        char temp[strlen(path) + 1]; //copy of the path
        strcpy(temp, path);

        int current = 0;
        char *token = strtok(temp + 1, "/"); //first component of path (string, delimiter)
        char *next = strtok(NULL, "/");

	while(next)
	{
		/*
		strcpy(next, token);
		token = strtok(NULL, "/");

		if (!token)
		{
			strcpy(child, next);
			return current;
		}
		*/

		int block, entry;
		if (!findDirent(current, token, &block, &entry))
		{
			return -1;
		}

		dirent *entries = (dirent *)blockPtr(block) + entry;
                if (entries[entry].type != 2)
		{
                        return -1;
		}

                current = entries[entry].firstblock;
		token = next;
		next = strtok(NULL, "/");
	}

	strcpy(child, token);
	child[255] = '\0';

	if (parent)
	{
		*parent = current;
	}
	
	return current;
}
/*
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
*/
void makeDirectory(const char *path)
{
	/*
	char child[256];
	int parent = pathTraverse(path, child);
	if (parent < 0)
	{
		fprintf(stdout, "bad path\n");
		return;
	}
	while (token)
	{
		int block, entry;
		if (findDirent(parent, child, &block, &entry))
		{
			fprintf(stdout, "directory is already here\n");
			return;
		}
	
		int new = fatBlockFinder();
		FAT[new] = USHRT_MAX;
		memset(blockPtr(new), 0, BLOCKSIZE);
		
		dirent d;
	        memset(&d, 0, sizeof(d));
	       	strcpy(d.name, child);
		d.name[255] = '\0';
	        d.type = 2;
	        d.size = 0;
	        d.firstblock = new;
	}
	if (addDirent(parent, &d) != 0)
	{
		fprintf(stdout, "couldn't add directory!!!!\n");
	}
	*/

 	if (!path || path[0] != '/') 
	{
        	fprintf(stdout, "bad path\n");
       		return;
	}

	char temp[strlen(path) + 1];
	strcpy(temp, path);

	int current = 0;  
	char *token = strtok(temp + 1, "/");

    	while (token) 
	{

        	int block, entry;

        	if (findDirent(current, token, &block, &entry)) 
		{
        	    dirent *e = (dirent *)blockPtr(block) + entry;
	
	            if (e->type != 2) 
		    {
	                printf("'%s' exists but is not a directory!\n", token);
	                return;
		    }
	
        	    current = e->firstblock;  
        }
        else 
	{
		int newblk = fatBlockFinder();
            	if (newblk < 0) 
		{
                	fprintf(stdout, "No space in FAT!\n");
                	return;
		}

            	FAT[newblk] = USHRT_MAX;
            	memset(blockPtr(newblk), 0, BLOCKSIZE);

            	dirent d;
            	memset(&d, 0, sizeof(d));
            	strcpy(d.name, token);
            	d.name[255] = '\0';
		d.type = 2;
            	d.size = 0;
            	d.firstblock = newblk;

            	if (addDirent(current, &d) != 0) 
		{
                	fprintf(stdout, "could not add directory '%s'\n", token);
                	return;
            	}

            	current = newblk;
        }

        token = strtok(NULL, "/");
    }

    fprintf(stdout, "Created directory path: %s\n", path);

}

void loadfs()
{
	struct header *h = (struct header*)fs;

	FAT = (unsigned short*)(fs + h->fatStart);

	fprintf(stdout, "Loaded filesystem!\n");
}

void printChain(int start)
{
	int block = start;
	int maxDirents = BLOCKSIZE / sizeof(dirent);

	while (block != USHRT_MAX)
	{
		dirent *entries = (dirent *)blockPtr(block);
		for (int i = 0; i < maxDirents; i++)
		{
			if (entries[i].name[0] != '\0')
			{
				fprintf(stdout,"%s \ttype: %d \tfirstblock: %d\n",entries[i].name, entries[i].type, entries[i].size, entries[i].firstblock);
			}
		}

		block = FAT[block];
	}
}

void lsfs()
{
	header *h = (header *)fs;
	fprintf(stdout, "Listing contents...\n");
	printChain(h->rootStart);
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
/*
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
*/
void addfilefs(char* fname)
{
	char child[256];
	int parent;

	if (pathTraverse(fname, child, &parent) < 0)
	{
		fprintf(stdout, "Invalid path!!!!!\n");
	}

	FILE* fileToAdd = fopen(child, "rb");
	if (!fileToAdd)
	{	
		fprintf(stdout, "no file to add\n");
		return;
	}
	struct stat fstat;
	if (stat(child, &fstat) != 0)
	{
		fprintf(stdout, "stat error");
		exit(1);
	}

	int fileSize = (int)fstat.st_size;
	int blocksNeeded = (fileSize + BLOCKSIZE - 1) / BLOCKSIZE;
	int first = buildChain(blocksNeeded);
	if (first < 0)
	{
		fprintf(stdout, "not enough space\n");
	}

	int current = first; //index of first data block
	int remain = fileSize;	//# of bytes left

	while (current != USHRT_MAX && remain > 0)
	{
		unsigned char *block = blockPtr(current);
		int read = (remain < BLOCKSIZE) ? remain : BLOCKSIZE;

		fread(blockPtr, 1, read, fileToAdd);
		remain -= read;
		current = FAT[current];
	}

	fclose(fileToAdd);

	dirent d;
	memset(&d, 0, sizeof(d));
	strcpy(d.name, child);
	d.name[255] = '\0';
	d.type = 1;
	d.size = fileSize;
	d.firstblock = first;

	if (addDirent(parent, &d) != 0)
	{
		fprintf(stdout, "Couldn't add file %s!!!!\n", child);
	}

	fprintf(stdout, "Added %s which used %d blocks! first block in chain: %d\n", fname, blocksNeeded, first);
}

void removefilefs(char* fname)
{
	char child[256];
	int parent;

	if (pathTraverse(fname, child, &parent) < 0)
        {
                fprintf(stdout, "no good path!!!!!\n");
		return;
        }

	int block, entry;
	if (!findDirent(parent, child, &block, &entry))
        {
                fprintf(stdout, "file is too weird\n");
                return;
        }

	dirent *d = (dirent *)blockPtr(block) + entry;

	int current = d->firstblock;
	while (current != USHRT_MAX)
	{
		int next = FAT[current];
		FAT[current] = 0;
		current = next;
	}

	memset(d, 0, sizeof(dirent));

	fprintf(stdout, "Removed %s\n", fname);

}


void extractfilefs(char* fname)
{
	char child[256];
	int parent;
	if (pathTraverse(fname, child, &parent) < 0)
	{
	       	fprintf(stdout,"path smells funny\n"); 
		return; 
	}

	int block, entry;
	if (!findDirent(parent, child, &block, &entry))
        {
                fprintf(stdout, "file is too weird\n");
                return;
        }
	
	dirent* d = (dirent*)blockPtr(block) + entry;
    	int current = d->firstblock;
    	int remain = d->size;

    	while (current != USHRT_MAX && remain > 0) 
	{
        	int chunk = (remain < BLOCKSIZE) ? remain : BLOCKSIZE;
        	write(STDOUT_FILENO, blockPtr(current), chunk);
        	remain -= chunk;
        	current = FAT[current];
    	}
}
