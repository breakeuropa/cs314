#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

typedef struct header{
  char name[256];
  int size;
}header;

#define LIST 1
#define ADD 2

int main(int argc, char** argv){
  FILE* archive = fopen(argv[1], "a+");
  FILE* filetoadd;
  char filetoaddname[256];
  char opt;
  int flags = 0;
  while ((opt = getopt(argc, argv, "a:l")) != -1) {
    switch (opt) {
    case 'l':
      flags |= LIST;
      break;
    case 'a':
      flags |= ADD;
      filetoadd = fopen(optarg,"r");
      strcpy(filetoaddname, basename(optarg));
      break;
    default: /* '?' */
      fprintf(stderr, "No\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }    

  if(flags & ADD){
    printf("got add flag %s\n", filetoaddname);
    (filetoadd==NULL)?printf("file not found\n"):printf("file found\n");
  }
 
  if(flags & LIST) printf("got list flag\n");

  //archive and filetoadd
  if(flags & ADD){
    header headertoadd;
    char buffer;
    strcpy(headertoadd.name, filetoaddname);
    fseek(filetoadd, 0, SEEK_END);
    headertoadd.size = ftell(filetoadd);
    fseek(filetoadd, 0, SEEK_SET);

    fwrite(&headertoadd, 1, sizeof(header), archive);
    while(fread(&buffer, 1, 1, filetoadd) != 0)
      fwrite(&buffer, 1, 1, archive);
 
  }

  if (flags & LIST){
    fseek(archive, 0, SEEK_END);
    int archsize = ftell(archive);
    char* myarchive = mmap(NULL, archsize, PROT_READ, MAP_SHARED, fileno(archive), 0);

    header* firstheader = (header*)myarchive;
    int datasize = firstheader->size;
    printf("%s %d\n", firstheader->name, firstheader->size);

    header* secondheader = (header*)(myarchive + sizeof(header) + datasize);
    printf("%s %d\n", secondheader->name, secondheader->size);
    
    
    /*    header myheader;
    fseek(archive, 0, SEEK_SET);
    while(fread(&myheader, 1, sizeof(header), archive) != 0){
      printf("%s %d\n", myheader.name, myheader.size);
      fseek(archive, myheader.size, SEEK_CUR);

    }
    */
  }
  
  return 0;
}
