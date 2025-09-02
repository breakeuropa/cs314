#include <stdlib.h>
#include <stdio.h>

void set(int* a, int val)
{
	*a = val;
}

int main()
{
	printf("Problem 1\n\n");
	int i = 5;
	int val = 15;
	int* a = &i;

	printf("a before set:%d\n", *a);
	set(a,val);
	printf("a after set:%d\n", *a);
	printf("\n");

	return 0;
}
