#include "tagptr.h"

#include "stdlib.h"

int main(int argc, char **argv) {
    tagptr arr;
    arr.ptr = NULL;
    tagarray_pprint(arr);
    tagptr tmp;
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
    set_tag(&tmp,36);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tmp.ptr = malloc(16);
    set_tag(&tmp,32);
    tagarray_push_back(&arr,tmp);
    tagarray_pprint(arr);

    tagarray_sort(arr);
    tagarray_pprint(arr);
    return 0;
}
