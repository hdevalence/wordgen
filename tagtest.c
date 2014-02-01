#include "tagptr.h"

#include "stdlib.h"

int main(int argc, char **argv) {
    tagptr_t arr;
    arr.ptr = NULL;
    tagarray_pprint(arr);
    tagptr_t tmp;

    tmp.ptr = malloc(16);
    set_tag(&tmp,5);
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,3);
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,1);
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,2);
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,4);
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tagarray_sort(arr);
    tagptr_t val = tagarray_search(arr, 4);
    tagarray_pprint(arr);
    tagarray_pprint(val);
    return 0;
}
