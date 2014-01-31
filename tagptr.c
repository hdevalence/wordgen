#include "tagptr.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

extern inline void* mask_ptr(tagptr p);
extern inline void set_tag(tagptr *p, uint16_t tag);
extern inline uint16_t get_tag(tagptr p);
extern inline tagptr tagarray_at(tagptr arr, int i);
extern inline void tagarray_set(tagptr arr, int i, tagptr obj);

void tagarray_push_back(tagptr* arr, tagptr p) {
    uint8_t size = arr->bytes[7];
    uint8_t reserved = arr->bytes[6];
    if (size == reserved) {
        //Resize array
        tagptr* raw_arr = (tagptr*)mask_ptr(*arr);
        int new_res = 2*reserved;
        if (new_res > TAGARRAY_MAXLEN)
            new_res = TAGARRAY_MAXLEN;
        if (new_res == 0)
            new_res = 1;
        tagptr* new_arr = realloc(raw_arr, sizeof(tagptr)*new_res);
        assert(new_arr);
        arr->ptr = new_arr;
        arr->bytes[7] = size;
        arr->bytes[6] = new_res;
    }
    tagarray_set(*arr,size,p);
    ++size;
    arr->bytes[7] = size;
}

void tagarray_swap(tagptr arr, int i, int j) {
    tagptr tmp = tagarray_at(arr, i);
    tagarray_set(arr, i, tagarray_at(arr, j));
    tagarray_set(arr, j, tmp);
}

/* Partition arr on the closed interval [l,r] with pivot index p. */
int tagarray_partition(tagptr arr, int l, int r, int p) {
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

void tagarray_qsort(tagptr arr, int l, int r) {
    if (l < r) {
        int p = (l+r)/2; // no overflow worries here
        int newp = tagarray_partition(arr,l,r,p);
        tagarray_qsort(arr,l,newp-1);
        tagarray_qsort(arr,newp+1,r);
    }
}

void tagarray_sort(tagptr arr) {
    int size = arr.bytes[7];
    tagarray_qsort(arr,0,size);
}

void tagarray_pprint(tagptr arr) {
    int size = arr.bytes[7];
    int reserved = arr.bytes[6];
    printf("PPRINT 0x%16llX\n", (unsigned long long) arr.ptr);
    printf("  SIZE %d\n", size);
    printf("  RSVD %d\n", reserved);
    for(int i = 0; i < size; ++i) {
        tagptr p = tagarray_at(arr,i);
        printf("\t%d\t0x%16llX\n", i, (unsigned long long) p.ptr);
        printf("\t\tTAG\t%d\n", get_tag(p));
        printf("\t\tVAL\t0x%16llX\n", (unsigned long long) mask_ptr(p));
    }
}
