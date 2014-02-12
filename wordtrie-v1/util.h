#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

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

// i-th digit of base 128 representation of val
inline uint8_t base128_digit(uint64_t val, int i) {
    return ((val >> 7*i) & 127);
}

/* Writes val to stream in variable length base-128 coding.
 * val is broken up into 7-bit segments written as bytes
 * to the stream, with the 8th bit indicating whether
 * there is more data to come.
 *
 * For instance, 111 01011000 is written as
 * 1 1011000
 * 0 0001110
 */
void put_base128(uint64_t val, FILE *stream);

/* Read a base-128 variable length uint64_t from the stream. */
uint64_t get_base128(FILE *stream);

#endif
