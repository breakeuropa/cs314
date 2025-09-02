#include <stdlib.h>
#include <stdio.h>

void swapPtrs(int** ptr1, int** ptr2)
{
	int* temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = temp;
}

int main()
{
	int val1 = 1;
	int val2 = 2;
	int* val1ptr = &val1;
	int* val2ptr = &val2;
	int** ptr1 = &val1ptr;
	int** ptr2 = &val2ptr;

	printf("\n\nProblem 3\n\n");

	printf("Before swap:\nval1: %d val1ptr: %d\nval2: %d val2ptr: %d\n", val1, val1ptr, val2, val2ptr);
	swapPtrs(ptr1, ptr2);
	printf("After swap:\nval1: %d val1ptr: %d\nval2: %d val2ptr: %d\n", val1, val1ptr, val2, val2ptr);
	printf("\n");
	return 0;
}
