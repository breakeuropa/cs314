#include <stdio.h>
#include <stdlib.h>

void swapInts(int* a, int* b)
{
	int temp = *a; //int temp set to the value thats at a's addy
	*a = *b;
	*b = temp;
}

int main ()
{
	int a = 7;
	int b = 49;
	int* ptra = &a;
	int* ptrb = &b;

	printf("\n\nProblem 2\n\n");

	printf("Before swap\na: %d\nb: %d\n", a,b);
	swapInts(ptra, ptrb);
	printf("After swap\na: %d\nb: %d\n", a,b);
	printf("\n");

	return 0;
}
