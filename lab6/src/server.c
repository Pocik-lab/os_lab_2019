#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "multmodulo.h"

struct FactorialArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

uint64_t Factorial(const struct FactorialArgs *args) 
{
  uint64_t ans = 1;

  for (int i = args->begin; i <= args->end; i++)
  {
      ans = MultModulo(ans, i, args->mod);
  }

  return ans;
}

void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  return (void *)(uint64_t *)Factorial(fargs);
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        // TODO: your code here
        if (port <= 0) {
          printf("port is a positive number\n");
          return 1;
        }
        break;
      case 1:
        tnum = atoi(optarg);
        // TODO: your code here
        if (tnum <= 0) {
          printf("tnum is a positive number\n");
          return 1;
        }
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  //Для создания сокета типа stream с протоколом TCP, обеспечивающим коммуникационную поддержку, вызов функции socket должен быть следующим
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  //Функция socket создает конечную точку для коммуникаций и возвращает файловый дескриптор, ссылающийся на сокет, или -1 в случае ошибки. Данный дескриптор используется в дальнейшем для установления связи
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

  //Структура sockaddr_in описывает сокет для работы с протоколами IP.
  struct sockaddr_in server;
  //Значение поля sin_family всегда равно AF_INET(IPv4)
  server.sin_family = AF_INET;
  //Поле sin_port содержит номер порта который намерен занять процесс. Если значение этого поля равно нулю, то операционная система сама выделит свободный номер порта для сокета.
  //Функция htons() преобразует узловой порядок расположения байтов положительного короткого целого hostshort в сетевой порядок расположения байтов.
  server.sin_port = htons((uint16_t)port);
  //Поле s_addr - этому полю можно присвоить 32х битное значение IP адреса.
  //Функция htonl() преобразует узловой порядок расположения байтов положительного целого hostlong в сетевой порядок расположения байтов.
  //INADDR_ANY все адреса локального хоста (0.0.0.0);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  //setsockopt - получить или установить флаги на сокете 
  //int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) -  возвращает 0 в случае успешного завершения,-1 в случае ошибки
  //Переменная sockfd должна ссылаться на открытый дескриптор сокета
  //Переменная level определяет, каким кодом должен интерпретироваться параметр: общими программами обработки сокетов или зависящими от протокола программами (например, IPv4, IPv6, TCP или SCTP)
  //Для присвоения параметра на уровне библиотеки сокетов, аргументу level присваивается значение SOL_SOCKET
  //SO_REUSEADDR - разрешает повторное использование локальных адресов (если данная возможность поддерживается используемым протоколом)
  //optval — это указатель на переменную, из которой извлекается новое значение параметра с помощью функции setsockopt или в которой сохраняется текущее значение параметра с помощью функции getsockopt. Размер этой переменной задается последним аргументом. Для функции setsockopt тип этого аргумента — значение, а для функции getsockopt — «значение-результат»
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  //Для связывания сокета с адресом и номером порта используют системный вызов bind
  //Первый параметр - сокет-дескриптор, который данная функция именует
  //Второй параметр - указатель на структуру имени сокета,тип которой зависит от домена.
  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) 
  {
    fprintf(stderr, "Can not bind to socket!");
    return 1;
  }

  //Со стороны сервера процесс установления связи сложнее. Когда сервер желает предложить один из своих сервисов, он связывает сокет с общеизвестным адресом, ассоциирующимся с данным сервисом, и пассивно слушает этот сокет
  //Первый аргумент - это дескриптор сокета
  //Второй аргуемент -  это максимальное количество запросов на установление связи, которые могут стоять в очереди, ожидая обработки сервером
  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("Server listening at %d\n", port);

  while (true) 
  {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    //accept() Используется для принятия запроса на установление соединения от удаленного хоста. Принимает следующие аргументы:
    //Первый аргуемент — дескриптор слушающего сокета на принятие соединения.
    //Второй аргуемент — указатель на структуру sockaddr, для принятия информации об адресе клиента.
    //Третий аргумент — указатель на socklen_t, определяющее размер структуры, содержащей клиентский адрес и переданной в accept(). Когда accept() возвращает некоторое значение, socklen_t указывает сколько байт структуры cliaddr использовано в данный момент.
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) 
    {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) 
    {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];

      //recv - чтение данных из сокета
      //Первый аргумент - сокет-дескриптор, из которого читаются данные
      //Второй и третий аргументы - соответственно, адрес и длина буфера для записи читаемых данных
      //Четвертый параметр - это комбинация битовых флагов, управляющих режимами чтения
      //Если аргумент flags равен нулю, то считанные данные удаляются из сокета
      //Функция возвращает число считанных байтов или -1 в случае ошибки. Следует отметить, что нулевое значение не является ошибкой. Оно сигнализирует об отсутствии записанных в сокет процессом-поставщиком данных.
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) 
      {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];

      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %llu %llu %llu\n", begin, end, mod);

      int part_size = (end - begin + 1) / tnum;
      struct FactorialArgs args[tnum];
      for (uint32_t i = 0; i < tnum; i++) {
        // TODO: parallel somehow
        args[i].begin = begin + i*part_size;
        if (i + 1 == tnum)
        {
            args[i].end = end;
        }
        else
        {
            args[i].end = begin + (i + 1)*part_size - 1;
        }
        args[i].mod = mod;

        if (pthread_create(&threads[i], NULL, ThreadFactorial,(void *)(args + i))) 
        {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
      for (uint32_t i = 0; i < tnum; i++) 
      {
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
        total = MultModulo(total, result, mod);
      }

      printf("Total: %llu\n", total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));

      //send - отправка данных
      //Первый аргумент - сокет-дескриптор, в который записываются данные
      //Второй и третий аргументы - соответственно, адрес и длина буфера с записываемыми данными
      //Четвертый параметр - это комбинация битовых флагов, управляющих режимами записи. Если аргумент flags равен нулю,то запись в сокет (и,соответственно, считывание ) происходит в порядке поступления байтов
      //Функция возвращает число записанных в сокет байтов ( в нормальном случае должно быть равно значению параметра sizeof(total) ) или -1 в случае ошибки. Отметим, что запись в сокет не означает, что данные приняты на другом конце соединения процессом-потребителем. Для этого процесс-потребитель должен выполнить функцию recv
      //Таким образом, функции чтения и записи в сокет выполняются асинхронно.
      
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    //Вызов shutdown вызывает "моментальное" отбрасывание всех стоящих в очереди данных
    //Первый аргумент функции - сокет-дескриптор, который должен быть закрыт
    //Второй аргумент - целое значение, указывающее, каким образом закрывается сокет
    //Если how равно SHUT_RDWR, то запрещены как прием, так и передача данных.  
    shutdown(client_fd, SHUT_RDWR);

    //Эта функция закрывает сокет и разрывает все соединения с этим сокетом
    //В отличие от функции shutdown функция close может дожидаться окончания всех операций с сокетом, обеспечивая "нормальное", а не аварийное закрытие соединений
    //Аргумент функции - закрываемый сокет-дескриптор.
    close(client_fd);
  }

  return 0;
}
