#include <stdio.h>
#include "swap.c"

int main(int argc, char **argv)
{
	char ch1 = 'a';
	char ch2 = 'b';

	Swap(&ch1, &ch2);

	printf("%c %c\n", ch1, ch2);
	return 0;
}
