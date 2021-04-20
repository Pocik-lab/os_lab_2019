#ifndef PARSUM_H
#define PARSUM_H

#include "sumutils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args);

#endif