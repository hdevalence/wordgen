/*
 * Tagged pointer implementation for AMD64.
 * Allows storage of a uint16_t 'tag' in a pointer value.
 *
 * Non-portable.
 */
#ifndef TAGPTR_H
#define TAGPTR_H

#include <stdint.h>

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

#endif
