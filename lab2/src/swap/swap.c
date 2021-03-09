#include "swap.h"
#include "stdlib.h"
#include "string.h"

void Swap(char *left, char *right)
{
    char temp = *left;
    *left = *right;
    *right = temp;
}   
