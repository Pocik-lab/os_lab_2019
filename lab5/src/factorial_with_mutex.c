#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <pthread.h>

struct FactorialArgs
{
    int mod;
    int begin;
    int end;
};

long long total = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *Part_factorial(void *args)
{
    struct FactorialArgs *factorial_args = (struct FactorialArgs *)args;

    pthread_mutex_lock(&mutex);

    printf("Started %d - %d\n", factorial_args->begin, factorial_args->end);

    for(int i = factorial_args->begin; i <= factorial_args->end; i++)
    {
        total = (total * i) % factorial_args->mod;
    }

    printf("Ended %d - %d\n", factorial_args->begin, factorial_args->end);

    pthread_mutex_unlock(&mutex);
    return (void *)NULL;
}

int main(int argc, char **argv)
{
  
  int pnum = -1;
  int mod = -1;
  int k = -1;
    while (true) 
    {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"pnum", required_argument, 0, 0},
                                      {"k", required_argument, 0, 'k'},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "k:", options, &option_index);

    if (c == -1) break;
    switch (c) 
    {
      case 0:
        switch (option_index) 
        {
          case 0:
            pnum = atoi(optarg);
            if (pnum <= 0) 
            {
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
            
          case 1:
            k = atoi(optarg);
            if (k < 0) 
            {
            printf("k is a positive number\n");
            return 1;
            }
            break;

          case 2:
            mod = atoi(optarg);
            if (mod <= 0) 
            {
                printf("mod is a positive number\n");
                return 1;
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case 'k':
        k = atoi(optarg);
        if (k < 0) 
        {
            printf("k is a positive number\n");
            return 1;
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) 
  {
    printf("Has at least one no option argument\n");
    return 1;
  }
  
  if (k == -1 || mod == -1 || pnum == -1) {
    printf("Usage: %s -k \"num\" --mod=\"num\" --pnum=\"num\" \n", argv[0]);
    return 1;
  }

  pthread_t threads[pnum];

  struct FactorialArgs args[pnum];
  int part_size = k / pnum;

  for (int i = 0; i < pnum; i++)
  {
    args[i].mod = mod;
    args[i].begin = i*part_size + 1;
    if (i + 1 == pnum)
    {
        args[i].end = k;
    }
    else 
    {
        args[i].end = (i + 1)*part_size;
    }
    if (pthread_create(&threads[i], NULL, Part_factorial, (void *)(args + i))) 
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  for (uint32_t i = 0; i < pnum; i++) 
  {
    pthread_join(threads[i], NULL);
  }

  printf("%d! = %lld\n", k, total);

  return 0;
}