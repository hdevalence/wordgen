#include "tagarray.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mempool.h"

extern inline tagptr_t tagarray_at(tagptr_t arr, int i);
extern inline void tagarray_set(tagptr_t arr, int i, tagptr_t obj);
extern inline uint8_t tagarray_size(tagptr_t arr);
extern inline uint8_t tagarray_reserved(tagptr_t arr);

void tagarray_free(tagptr_t arr)
{
    uint8_t reserved = tagarray_reserved(arr);
    if (reserved) {
        tagptr_t *raw_arr = (tagptr_t*)mask_ptr(arr);
        mempool_free(mempool(sizeof(tagptr_t)*reserved), raw_arr);
    }
}

void tagarray_push_back(tagptr_t* arr, tagptr_t p) {
    uint8_t size = arr->bytes[7];
    uint8_t reserved = arr->bytes[6];
    if (size == reserved) {
        //Resize array
        tagptr_t* raw_arr = (tagptr_t*)mask_ptr(*arr);
        int new_res = 2*reserved;
        if (new_res > TAGARRAY_MAXLEN) {
            new_res = TAGARRAY_MAXLEN;
            // Trying to expand the array past max size?
            // You will have a bad day.
            assert(size < TAGARRAY_MAXLEN);
        }
        if (new_res == 0)
            new_res = 1;
        // Reallocate the array into a new memory pool
        tagptr_t* new_arr = mempool_alloc(mempool(sizeof(tagptr_t)*new_res));
        memcpy(new_arr, raw_arr, sizeof(tagptr_t)*reserved);
        if (reserved)
            mempool_free(mempool(sizeof(tagptr_t)*reserved), raw_arr);
        // Update array tagpointer
        arr->ptr = new_arr;
        arr->bytes[7] = size;
        arr->bytes[6] = new_res;
    }
    tagarray_set(*arr,size,p);
    ++size;
    arr->bytes[7] = size;
}

void tagarray_insert(tagptr_t* arr, tagptr_t p) {
    tagarray_push_back(arr,p);
    // If we only have one element, no-op
    if (tagarray_size(*arr) <= 1)
        return;
    /* Now p is at the end of arr: perform insertion sort.
     * This is asymptotically bad (insertion sort on every addition)
     * but all of our lists are quite small, so the constant
     * factors may make the difference.
     */
    int i = tagarray_size(*arr) - 1;
    while (i > 0) {
        tagptr_t q = tagarray_at(*arr,i-1);
        if( get_tag(q) > get_tag(p) ) {
            tagarray_set(*arr,i,q);
            --i;
        } else break;
    }
    tagarray_set(*arr,i,p);
}

void tagarray_swap(tagptr_t arr, int i, int j) {
    tagptr_t tmp = tagarray_at(arr, i);
    tagarray_set(arr, i, tagarray_at(arr, j));
    tagarray_set(arr, j, tmp);
}

/* Partition arr on the closed interval [l,r] with pivot index p. */
int tagarray_partition(tagptr_t arr, int l, int r, int p) {
    uint16_t pivot_tag = get_tag(tagarray_at(arr, p));
    tagarray_swap(arr, p, r);
    int store_idx = l;
    for (int i = l; i <= r; ++i) {
        if (pivot_tag > get_tag(tagarray_at(arr, i))) {
            tagarray_swap(arr,i,store_idx);
            store_idx++;
        }
    }
    tagarray_swap(arr,store_idx,r);
    return store_idx;
}

void tagarray_qsort(tagptr_t arr, int l, int r) {
    if (l < r) {
        int p = (l+r)/2; // no overflow worries here
        int newp = tagarray_partition(arr,l,r,p);
        tagarray_qsort(arr,l,newp-1);
        tagarray_qsort(arr,newp+1,r);
    }
}

void tagarray_sort(tagptr_t arr) {
    int size = arr.bytes[7];
    tagarray_qsort(arr,0,size-1);
}

tagptr_t tagarray_search(tagptr_t arr, uint16_t search_tag) {
    int l = 0;
    int r = tagarray_size(arr);
    while (l < r) {
        int mid = (l+r)/2;
        uint16_t mid_tag = get_tag(tagarray_at(arr,mid));
        if (mid_tag < search_tag) {
            l = mid + 1;
        } else if (mid_tag > search_tag) {
            r = mid;
        } else {
            return tagarray_at(arr,mid);
        }
    }
    tagptr_t not_found = { .ptr = NULL };
    return not_found;
}

void tagarray_pprint(tagptr_t arr) {
    int size = arr.bytes[7];
    int reserved = arr.bytes[6];
    printf("PPRINT 0x%016llX\n", (unsigned long long) arr.ptr);
    printf("  SIZE %d\n", size);
    printf("  RSVD %d\n", reserved);
    for(int i = 0; i < size; ++i) {
        tagptr_t p = tagarray_at(arr,i);
        printf("\t%d\t0x%016llX\n", i, (unsigned long long) p.ptr);
        printf("\t\tTAG\t%d : %c\n", get_tag(p), (char)get_tag(p));
        printf("\t\tVAL\t0x%016llX\n", (unsigned long long) mask_ptr(p));
    }
}
