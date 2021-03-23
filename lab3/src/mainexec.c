#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	pid_t pid = fork();

	if (pid == 0)
	{
	execlp("main_min_max",argv[0],argv[1], NULL);
	}
	else
	{
		printf("error");
	}

	return 0;
}
