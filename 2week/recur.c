#include <stdio.h>

void foo(int in)
{
	char a[1000000] = {0};
	printf("%d\n", in);
	foo(++in);
}

int main()
{
	foo(0);
	return 0;
}
