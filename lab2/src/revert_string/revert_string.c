#include <string.h>
#include <stdlib.h>
#include "revert_string.h"

void RevertString(char *str)
{

    char *c;
    c = (char*)malloc(sizeof(str));
	for (int i = 0, j = strlen(str) - 1; i < j; i++,  j--)
    {
        *c = str[i];
        str[i] = str[j];
        str[j] = *c;
    }
}

