#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void seedrand();

inline void err_oom(char *msg)
{
    fprintf(stderr,"Out of memory: malloc failed in\t%s\n", msg);
    exit(EXIT_FAILURE);
}

/* Given an array summing to arraysum, sample from its probability
 * distribution (normalizing by arraysum).
 *
 * You should call seedrand() first.
 */
int random_sample(int arraylen, double arraysum, double *array);

/* Returns true if c is in allowed chars or allowed_chars is NULL */
inline bool in_string(char c, char *allowed_chars) {
    // TODO: is this even necessary over strstr or strcasestr?
    if (!allowed_chars) {
        return true;
    } else {
        while (*allowed_chars != 0) {
            if (*allowed_chars == c)
                return true;
            ++allowed_chars;
        }
        return false;
    }
}

inline double sum(int count, double *restrict arr) {
    double s = 0;
    for (int i = 0; i < count; ++i)
        s += arr[i];
    return s;
}


#endif
