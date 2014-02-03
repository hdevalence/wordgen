#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

inline void err_oom(char *msg)
{
    fprintf(stderr,"Out of memory: malloc failed in\t%s\n", msg);
    exit(EXIT_FAILURE);
}

#endif
