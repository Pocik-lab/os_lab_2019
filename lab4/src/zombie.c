#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	//Создание дочернего процесса
	pid_t child_pid;
	child_pid = fork();

	if (child_pid > 0)
	{
		//Делаем минутную паузу в родительском процессе
		sleep(60);
	}
	else
	{
		//Закрываем немедленно дочерний процесс
		exit(0);
	}

	return 0;
}

