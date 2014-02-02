#include "wordtrie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern inline bool wtrie_valid_char(char c);

wtrie_t* wtrie_alloc() {
    wtrie_t *n = malloc(sizeof(wtrie_t));
    if (n == NULL) {
        fprintf(stderr,"malloc failed\n");
        exit(EXIT_FAILURE);
    }
    tagptr_t no_children = { .ptr = NULL };
    n->child_arr = no_children;
    n->self_freq = 0;
    n->children_freq = 0;
    return n;
}

void wtrie_add_entry(wtrie_t *root, char *key, uint64_t freq) {
    char k = *key;
    if (k) {
        /* Not done: recurse, constructing nodes as needed. */
        tagptr_t child = tagarray_search(root->child_arr, k);
        if (!child.ptr) {
            child.ptr = wtrie_alloc();
            set_tag(&child, k);
            tagarray_insert(&(root->child_arr), child);
        }
        wtrie_add_entry((wtrie_t*)mask_ptr(child), key+1, freq);
    } else {
        /* If we're at the end of the key string, then root is 
         * actually the leaf for the given key. */
        root->self_freq += freq;
        return;
    }
}

wtrie_t* wtrie_find_entry(wtrie_t *root, char *key) {
    if (!(*key))
        return root;
    else {
        tagptr_t child = tagarray_search(root->child_arr, *key);
        if (child.ptr)
            return wtrie_find_entry((wtrie_t*)mask_ptr(child),key+1);
        else
            return NULL;
    }
}

void wtrie_compute_freq(wtrie_t *root) {
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        wtrie_compute_freq(child);
        root->children_freq += child->self_freq;
        root->children_freq += child->children_freq;
    }
}

// FIXME: code duplication
// it'd be good to add some functional traversals

uint64_t count_leaves(wtrie_t *root) {
    int numchildren = tagarray_size(root->child_arr);
    if (numchildren == 0)
        return  1; // This is a leaf
    uint64_t count = 0;
    for (int i = 0; i < numchildren; ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_leaves(child);
    }
    return count;
}

uint64_t count_children(wtrie_t *root) {
    uint64_t count = 1;
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_children(child);
    }
    return count;
}

uint64_t count_wasted_mem(wtrie_t *root) {
    uint64_t count = tagarray_reserved(root->child_arr) - tagarray_size(root->child_arr);
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_wasted_mem(child);
    }
    return count;
}

void wtrie_free(wtrie_t *root) {
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        wtrie_free(child);
    }
    free((tagptr_t*)mask_ptr(root->child_arr));
    free(root);
}

void wtrie_pprint_helper(wtrie_t *root, char *prefix) {
    char *newprefix = malloc(strlen(prefix)+1+2);
    newprefix[0] = ' ';
    newprefix[1] = ' ';
    strcpy(newprefix+2,prefix);
    for(int i = 0; i < tagarray_size(root->child_arr); ++i) {
        tagptr_t child = tagarray_at(root->child_arr,i);
        wtrie_t *rawchild = (wtrie_t*)mask_ptr(child);
        printf("%s%c:  %lu  %lu\n",
                prefix, (char)get_tag(child), rawchild->self_freq, rawchild->children_freq);
        wtrie_pprint_helper((wtrie_t*)mask_ptr(child),newprefix);
    }
    free(newprefix);
}

void wtrie_pprint(wtrie_t *root) {
    printf("self_freq = %lu    children_freqs = %lu\n", root->self_freq, root->children_freq);
    char prefix = 0;
    wtrie_pprint_helper(root,&prefix);
}















