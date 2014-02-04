#include <stdlib.h>
#include <stdio.h>

#include "tagptr.h"
#include "tagarray.h"

int main() {
    tagptr_t arr;
    arr.ptr = NULL;
    tagarray_pprint(arr);
    tagptr_t tmp;

    tmp.ptr = malloc(16);
    set_tag(&tmp,'e');
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,'b');
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,'c');
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,'a');
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,'d');
    tagarray_insert(&arr,tmp);
    tagarray_pprint(arr);

    tagarray_sort(arr);
    tagptr_t val = tagarray_search(arr, 'd');
    tagarray_pprint(arr);
    tagarray_pprint(val);
    return 0;
}
