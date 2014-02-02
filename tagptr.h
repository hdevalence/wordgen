#ifndef TAGPTR_H
#define TAGPTR_H

#include <stdint.h>

#define TAGARRAY_MAXLEN 95

/*
 * Union that allows access to pointer data bytewise.
 *
 * Since the x64 architecture uses only 48 bits of the address space,
 * we can use the 16 high bits to hold a tag, as long as we mask away
 * the tag before dereferencing the pointer.
 *
 * Note that this implementation assumes that the high bits are all
 * zeros; i.e., that the pointer points to somewhere in heap space.
 * It also (obviously) assumes that the code is run on x64.
 */
union tagptr_t {
    void *ptr;
    uint16_t tags[4];
    uint8_t bytes[8];
};
typedef union tagptr_t tagptr_t;

/***********************************************************************
 *** Basic usage
 ***
 *** Create a tagptr_t and then initialize it with the pointer value.
 *** You can then set a tag on the pointer, read it, etc.
 ***
 *** Important: Never dereference the pointer! Always use mask_ptr.
 **/

/*
 * Mask the tag from a pointer to get something dereferenceable.
 *
 * You should then cast the pointer to the appropriate type.
 */
inline void * mask_ptr(tagptr_t p) {
    return (void*)(((uint64_t)p.ptr) & 0x0000ffffffffffffULL);
}

inline void set_tag(tagptr_t *p, uint16_t tag) {
    p->tags[3] = tag;
}

inline uint16_t get_tag(tagptr_t p) {
    return p.tags[3];
}

/***********************************************************************
 *** Tagged pointer lists
 ***
 *** These functions allow use of tagged pointers as a space-efficient
 *** dynamically sized associative array, provided the array keys are
 *** distinct.
 ***
 *** The array size, bounded by TAGARRAY_MAXLEN, is stored in bytes[7],
 *** while the size of the underlying buffer (the reserved space)
 *** is stored in bytes[6].
 ***
 *** This has the *extremely* convenient property that the pointer to
 *** the array is the array object itself.
 ***
 *** For instance, NULL is interpreted as an empty array.
 ***
 **/

/*
 * Given a tagged pointer array arr, append p to the array.
 *
 * This function will resize arr as needed up to TAGARRAY_MAXLEN.
 * It is the caller's responsibility to ensure that this does not occur.
 *
 * Note that this also serves as a method for creating arrays:
 * simply append to a null pointer.
 */
void tagarray_push_back(tagptr_t* arr, tagptr_t p);

/*
 * Similar to tagarray_push_back, but preserves order on the array.
 */
void tagarray_insert(tagptr_t* arr, tagptr_t p);

/*
 * Sort a tagged array by tag value.
 */
void tagarray_sort(tagptr_t arr);

/*
 * Search a sorted tagged array for the pointer with a given tag.
 */
tagptr_t tagarray_search(tagptr_t arr, uint16_t searchtag);

inline tagptr_t tagarray_at(tagptr_t arr, int i) {
    return ((tagptr_t*)mask_ptr(arr))[i];
}

inline void tagarray_set(tagptr_t arr, int i, tagptr_t obj) {
    ((tagptr_t*)mask_ptr(arr))[i] = obj;
}

inline uint8_t tagarray_size(tagptr_t arr) {
    return arr.bytes[7];
}

inline uint8_t tagarray_reserved(tagptr_t arr) {
    return arr.bytes[6];
}

/*
 * Pretty-print a tagged array for debugging.
 */
void tagarray_pprint(tagptr_t arr);




#endif
