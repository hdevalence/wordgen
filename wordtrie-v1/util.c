#define _XOPEN_SOURCE 600
#include "util.h"

#include <sys/time.h>
#include <stdlib.h>

extern inline void err_oom(char *msg);
extern inline double sum(int count, double *restrict arr);
extern inline bool in_string(char c, char *allowed_chars);

void seedrand() {
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);
    srand48(t.tv_usec * t.tv_sec);
}

int random_sample(int arraylen, double arraysum, double *array) {
    double sampled = arraysum*drand48();
    double cumsum = 0;
    for (int i = 0; i < arraylen; ++i) {
        cumsum += array[i];
        if (sampled < cumsum)
            return i;
    }
    // Should only happen if sampled == arraysum, i.e., almost never
    return arraylen-1;
}

