#include <stdio.h>
#include <stdlib.h>

int global = 1;

void fun()
{
	static int val = 0;
	int localval = 0;
	val ++;
	localval ++;
	printf("(%p) : %d\n(%p) : %d\n", &val, val, &localval, localval);
}

int main()
{
	int* pint = malloc(sizeof(int)); //returns pointer, addr on the heap
	pint = (int*)12;

	fun();
	fun();
	return 0;
}
