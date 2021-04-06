#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	pid_t pid = fork();

	if (pid == 0)
	{
	execl("main_min_max"," ",argv[1],argv[2], NULL);
	}
	else
	{
		return 0;
	}
}
