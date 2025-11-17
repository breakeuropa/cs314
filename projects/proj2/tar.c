#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define ADD 1
#define LIST 2

typedef struct header{
  char name[256];
  int size;
}header;

int main(int argc, char* argv[]){
  char* infile;
  char opt;

  FILE* archive = fopen(argv[1], "a+");

  int flags = 0;
  while ((opt = getopt(argc, argv, "a:l")) != -1) {
    switch (opt) {
    case 'l':
      flags = flags | LIST;
      break;
    case 'a':
      flags = flags | ADD;
      infile = (optarg);
      ;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  
  if (flags & LIST) printf("list flag is on\n");
  if (flags & ADD) printf("add flag is on with %s\n", infile);


  if (flags & ADD){
    header *newheader = malloc(sizeof(header));
    FILE* in = fopen(infile, "r");
    strcpy(newheader->name, infile);

    fseek(in, 0, SEEK_END);
    newheader->size = ftell(in);
    fseek(in, 0, SEEK_SET);
    char buffer;
    fwrite(newheader, 1, sizeof(header), archive);
    while(fread(&buffer, 1, 1, in) != 0){
      fwrite(&buffer, 1, 1, archive);
    }
  }

  if (flags & LIST){
    fseek(archive, 0, SEEK_END);
   
    char* archmap = mmap(NULL, ftell(archive), PROT_READ, MAP_PRIVATE, fileno(archive), 0);

    header* myheader = (header*)archmap;
    printf("%s %d\n", myheader->name, myheader->size);

    int datasize = myheader->size;
    
    myheader = (header*)(archmap+sizeof(header)+datasize);
    printf("%s %d\n", myheader->name, myheader->size);
    
    /*header myheader;

      while(fread(&myheader, 1, sizeof(header), archive) != 0){
      printf("%s %d\n", myheader.name, myheader.size);
      fseek(archive, myheader.size, SEEK_CUR);
      }*/
    
  }
  
  return 0;
}
