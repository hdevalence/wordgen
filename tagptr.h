#ifndef TAGPTR_H
#define TAGPTR_H

#include <stdint.h>

#define TAGARRAY_MAXLEN 95

union tagptr_t {
    void *ptr;
    uint16_t tags[4];
    uint8_t bytes[8];
};
typedef union tagptr_t tagptr_t;

inline void * mask_ptr(tagptr_t p) {
    return (void*)(((uint64_t)p.ptr) & 0x0000ffffffffffffULL);
}

inline void set_tag(tagptr_t *p, uint16_t tag) {
    p->tags[3] = tag;
}

inline uint16_t get_tag(tagptr_t p) {
    return p.tags[3];
}

/*
 * Add a new tagged pointer to an array of tagged pointers.
 * The array is itself a tagged pointer.
 */
void tagarray_push_back(tagptr_t* arr, tagptr_t p);

/* Like push_back, but maintains sort order. */
void tagarray_insert(tagptr_t* arr, tagptr_t p);

inline tagptr_t tagarray_at(tagptr_t arr, int i) {
    return ((tagptr_t*)mask_ptr(arr))[i];
}

inline void tagarray_set(tagptr_t arr, int i, tagptr_t obj) {
    ((tagptr_t*)mask_ptr(arr))[i] = obj;
}

inline uint8_t tagarray_size(tagptr_t arr) {
    return arr.bytes[7];
}

void tagarray_pprint(tagptr_t arr);

/*
 * Sort a tagged array by the tag
 */
void tagarray_sort(tagptr_t arr);

/*
 * Search a tagged array for the pointer with a given tag.
 * Assumed to be a presorted array.
 */
tagptr_t tagarray_search(tagptr_t arr, uint16_t searchtag);




#endif
