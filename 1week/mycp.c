#include <stdio.h>

int main(int argc, char** argv)
{
  FILE* src = fopen(argv[1], "r");
  FILE* dest = fopen(argv[2], "w");

  char buffer;

  while (fread(&buffer, 1, 1, src))
  {
    fwrite(&buffer, 1, 1, dest);
  }
      
  return 0;
}

