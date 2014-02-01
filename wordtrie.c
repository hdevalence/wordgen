#include "wordtrie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern inline bool valid_char(char c);

bool valid_key(const char* str) {
    while (*str) {
        if (!valid_char(*str))
            return false;
        str++;
    }
    return true;
}

wtrie_t* alloc_wordtrie() {
    wtrie_t *n = malloc(sizeof(wtrie_t));
    assert(n != NULL);
    tagptr_t no_children = { .ptr = NULL };
    n->child_arr = no_children;
    n->self_freq = 0;
    n->children_freq = 0;
    return n;
}

void add_entry(wtrie_t *root, char *key, uint64_t freq) {
    if (!(*key)) {
        /* If we're at the end of the key string, then root is 
         * actually the leaf for the given key. */
        root->self_freq += freq;
        return;
    } else {
        /* Otherwise, recurse, constructing nodes as needed. */
        tagptr_t child = tagarray_search(root->child_arr, *key);
        if (!child.ptr) {
            child.ptr = alloc_wordtrie();
            set_tag(&child, *key);
            tagarray_insert(&(root->child_arr), child);
        }
        add_entry((wtrie_t*)mask_ptr(child), key+1, freq);
    }
}

wtrie_t* find_entry(wtrie_t *root, char *key) {
    if (!(*key))
        return root;
    else {
        tagptr_t child = tagarray_search(root->child_arr, *key);
        if (child.ptr)
            return find_entry((wtrie_t*)mask_ptr(child),key+1);
        else
            return NULL;
    }
}

void compute_children_freqs(wtrie_t *root) {
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        compute_children_freqs(child);
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
    uint64_t count = sizeof(tagptr_t*)*(root->child_arr.bytes[6] - root->child_arr.bytes[7]);
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_wasted_mem(child);
    }
    return count;
}

void free_wordtrie(wtrie_t *root) {
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        free_wordtrie(child);
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
        printf("%s%c:  %llu  %llu\n",
                prefix, (char)get_tag(child), rawchild->self_freq, rawchild->children_freq);
        wtrie_pprint_helper((wtrie_t*)mask_ptr(child),newprefix);
    }
    free(newprefix);
}

void wtrie_pprint(wtrie_t *root) {
    printf("self_freq = %llu    children_freqs = %llu\n", root->self_freq, root->children_freq);
    char prefix = 0;
    wtrie_pprint_helper(root,&prefix);
}















