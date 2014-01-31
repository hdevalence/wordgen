#ifndef TAGPTR_H
#define TAGPTR_H

#include <stdint.h>

#define TAGARRAY_MAXLEN 95

union tagptr {
    void *ptr;
    uint16_t tags[4];
    uint8_t bytes[8];
};
typedef union tagptr tagptr;

inline void * mask_ptr(tagptr p) {
    return (void*)(((uint64_t)p.ptr) & 0x0000ffffffffffffULL);
}

inline void set_tag(tagptr *p, uint16_t tag) {
    p->tags[3] = tag;
}

inline uint16_t get_tag(tagptr p) {
    return p.tags[3];
}

/*
 * Add a new tagged pointer to an array of tagged pointers.
 * The array is itself a tagged pointer.
 */
void tagarray_push_back(tagptr* arr, tagptr p);

inline tagptr tagarray_at(tagptr arr, int i) {
    return ((tagptr*)mask_ptr(arr))[i];
}

inline void tagarray_set(tagptr arr, int i, tagptr obj) {
    ((tagptr*)mask_ptr(arr))[i] = obj;
}

inline uint8_t tagarray_size(tagptr arr) {
    return arr.bytes[7];
}

void tagarray_pprint(tagptr arr);

/*
 * Sort a tagged array by the tag
 */
void tagarray_sort(tagptr arr);

/*
 * Search a tagged array for the pointer with a given tag.
 * Assumed to be a presorted array.
 */
//tagptr tagarray_search(tagptr arr, uint16_t searchtag);




#endif
