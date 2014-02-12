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

extern inline int base128_count(uint64_t val);

extern inline uint8_t base128_digit(uint64_t val, int i);

void put_base128(uint64_t val, FILE *stream)
{
    uint8_t digit = base128_digit(val,0);
    /* Loops until we finish writing */
    for(int i = 0; ; ++i) {
        uint8_t next_digit = base128_digit(val,i+1);
        /* If there's more data, set flag bit and continue. */
        if (next_digit != 0) {
            digit |= 128;
            fputc(digit, stream);
            digit = next_digit;
        } else {
            fputc(digit, stream);
            return;
        }
    }
}

uint64_t get_base128(FILE *stream)
{
    uint64_t val = 0;
    for(int i = 0; ; ++i) {
        uint8_t digit = fgetc(stream);
        val |= (digit & 127) << 7*i;
        if (!(digit & 128))
            break;
    }
    return val;
}


