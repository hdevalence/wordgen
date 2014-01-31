#include "tagptr.h"

#include "stdlib.h"

int main(int argc, char **argv) {
    tagptr_t arr;
    arr.ptr = NULL;
    tagarray_pprint(arr);
    tagptr_t tmp;

    tmp.ptr = malloc(16);
    set_tag(&tmp,34);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,24);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,14);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,99);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,32);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tagarray_sort(arr);
    tagptr_t val = tagarray_search(arr, 99);
    tagarray_pprint(arr);
    tagarray_pprint(val);
    return 0;
}
