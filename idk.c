#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mystruct
{
	int a;
	char stuff[256];
}mystruct;

char astring[] = "this is a string";

int main()
{
	mystruct *tostore = malloc(sizeof(mystruct)*100);
	//FILE* myfile = fopen("test", "a+");
	
	for (int i = 0; i < 100; i++)
	{
		tostore[i].a = i;
		strcpy(tostore[i].stuff, astring);
		
	}
	for (int i = 0; i < 100; i++)
	{
		printf("%d %s\n", tostore[i].a, tostore[i].stuff);
	}


	FILE* myfile = fopen("test", "w");
	for (int i = 0; i < 100; i++)
	{
		fwrite(&(tostore[i]), sizeof(mystruct), 1, myfile);
	}
	return 0;
}
