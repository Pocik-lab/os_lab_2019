#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "multmodulo.h"

struct ParallelArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  int sck;
};

uint64_t GetAnswer(const struct ParallelArgs *args) 
{
  char task[sizeof(uint64_t) * 3];
    memcpy(task, &args->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &args->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &args->mod, sizeof(uint64_t));

    if (send(args->sck, task, sizeof(task), 0) < 0) 
    {
        fprintf(stderr, "Send failed\n");
        exit(1);
    }
    

    char response[sizeof(uint64_t)];
    if (recv(args->sck, response, sizeof(response), 0) < 0) 
    {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    uint64_t cur_answer = 0;
    memcpy(&cur_answer, response, sizeof(uint64_t));

  return cur_answer;
}

void *ThreadServer(void *args) {
  struct ParallelArgs *fargs = (struct ParallelArgs *)args;
  return (void *)(uint64_t *)GetAnswer(fargs);
}

struct Server {
  char ip[255];
  int port;
};

//64-битное целое число без знака
bool ConvertStringToUI64(const char *str, uint64_t *val) 
{
  char* end = NULL;

  //Преобразование строки в значение типа unsigned long int. Анализируя строку string, strtoul интерпретирует её содержимое в без знаковое  целое число типа unsigned long int.
  //Параметры:
  //string - Си-строка для выполнения преобразования.
  //endptr - Ссылка на объект типа char*, значение которой содержит адрес следующего символа в строке string, сразу после предыдущего найденного числа. Если этот параметр не используется, он должен быть нулевым указателем.
  //basis - Основание системы исчисления.

  unsigned long long i = strtoull(str, &end, 10);

  //Если правильное значение выходит за диапазон принимаемых значений, возвращается LONG_MAX или LONG_MIN, и глобальная переменная ERRNO устанавливается в ERANGE. 
  if (errno == ERANGE) 
  {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  //Потому что 255 - максимальная длина имени файла
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) 
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        if (k < 0) {
                printf("k should be positive number\n");
                return 1;
        }
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        if (mod <= 0) {
                printf("mod should be positive number\n");
                return 1;
        }
        break;
      case 2:
        // TODO: your code here
        //access проверяет, имеет ли процесс права на чтение или запись, или же просто проверяет, существует ли файл (или другой объект файловой системы), с именем pathname. Если pathname является символьной ссылкой, то проверяются права доступа к файлу, на который она ссылается.
        //R_OK, W_OK и X_OK запрашивают соответственно проверку существования файла и возможности его чтения, записи или выполнения. 
        if(access(optarg, R_OK))
        {
            printf("Cannot access file\nFile path format: /path/to/file\n");
            return 1;
        }
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n", argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file
  FILE *fp;
  fp = fopen(servers, "r");
  unsigned int servers_num = 1;
  fscanf(fp, "%d\n", &servers_num);
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  for (int i = 0; i < servers_num; i++)
  {
      fscanf(fp, "%s : %d\n", &to[i].ip, &to[i].port);
  }

  // TODO: work continiously, rewrite to make parallel
  pthread_t threads[servers_num];
  struct ParallelArgs args[servers_num];
  uint64_t answer = 1;
  uint64_t part_size = k / servers_num;
  for (int i = 0; i < servers_num; i++) 
  {
    //Получает информацию о хосте по его имени. Результат работы помещается в специальную структуру hostent
    //В эту функцию надо передать имя хоста. Если функция выполнится неудачно или с ошибкой, то вернется NULL. Иначе указатель на структуру. Вы не должные изменять эту структуру.
    //Структура hostent используется функциями, чтобы хранить информацию о хосте: его имя, тип,IP адрес, и т.д. Вы никогда не должны пытаться изменять эту структуру или освобождать любой из компонентов. Кроме того, только одна копия структуры hostent должна быть связана с потоком.
    
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL)
    {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) 
    {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    //Устанавливает соединение с сервером
    //Первый аргумент - сокет-дескриптор клиента
    //Второй аргумент - указатель на адрес сервера (структура sockaddr) для соответствующего домена
    //Третий аргумент - целое число - длина структуры адреса.
    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) 
    {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // TODO: for one server
    // parallel between servers
    
    args[i].begin = i*part_size + 1;
    if (i + 1 == servers_num)
    {
        args[i].end = k;
    }
    else
    {
        args[i].end = (i + 1)*part_size;
    }
    args[i].mod = mod;
    args[i].sck = sck;
    if (pthread_create(&threads[i], NULL, ThreadServer, (void *)(args + i))) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  for (uint32_t i = 0; i < servers_num; i++) {
    uint64_t result = 0;
    pthread_join(threads[i], (void **)&result);
    answer = MultModulo(answer, result, mod);
    close(args[i].sck);
  }
  printf("Answer: %lu\n", answer);
  free(to);

  return 0;
}
