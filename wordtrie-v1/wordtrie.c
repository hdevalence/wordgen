// Needed for drand48()
#define _XOPEN_SOURCE 600
#include "wordtrie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tagarray.h"
#include "util.h"
#include "mempool.h"

extern inline bool wtrie_valid_char(unsigned char c);
extern inline unsigned char ascii_index(unsigned char c);
extern inline unsigned char index_ascii(unsigned char c);

static mempool_t *wtrie_pool;

wtrie_t* wtrie_alloc() {
    if (!wtrie_pool) {
        // Create 96 MB pool: 24 * 2^22
        wtrie_pool = create_mempool(sizeof(wtrie_t),(1 << 22));
    }
    wtrie_t *n = mempool_alloc(wtrie_pool);
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

char wtrie_sample_one(wtrie_t *root, char *allowed_chars, wtrie_t **sampled_child) {
    /* Initialize arrays to hold conditional distributions.
     * Note that we are sampling from
     * (children \cap allowed) x {stop_string, continue_string};
     * in other words, when we select a child node we need to know if we're
     * actually selecting it as a leaf node or as an intermediate on
     * the path to some other leaf.
     */
    double child_freqs[NUMCHARS];
    double self_freqs[NUMCHARS];
    for (int i = 0; i < NUMCHARS; ++i) {
        child_freqs[i] = 0;
        self_freqs[i] = 0;
    }
    /* Add children of *root to the probability distributions,
     * if their keys are in the allowed set */
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        tagptr_t child = tagarray_at(root->child_arr, i);
        wtrie_t *child_ptr = (wtrie_t*)mask_ptr(child);
        char key = get_tag(child);
        if (in_string(key,allowed_chars)) {
            child_freqs[ascii_index(key)] = child_ptr->children_freq;
            self_freqs[ascii_index(key)] = child_ptr->self_freq;
        }
    }
    /* Select whether to select as a leaf node or an internal node */
    double child_sum = sum(NUMCHARS, child_freqs);
    double self_sum = sum(NUMCHARS, self_freqs);
    char key;
    if ((self_sum+child_sum)*drand48() <= child_sum) {
        key = index_ascii(random_sample(NUMCHARS, child_sum, child_freqs));
        *sampled_child = (wtrie_t*)mask_ptr(tagarray_search(root->child_arr, key));
    } else {
        key = index_ascii(random_sample(NUMCHARS, self_sum, self_freqs));
        *sampled_child = NULL;
    }
    return key;
}

char *wtrie_sample_string(wtrie_t *root, char *allowed_chars) {
    #define SAMPLESTR_MAXLEN 512
    char *result = malloc(SAMPLESTR_MAXLEN);
    memset(result, 0, SAMPLESTR_MAXLEN);
    for (int i = 0; i < SAMPLESTR_MAXLEN; ++i) {
        wtrie_t *sampled_child = NULL;
        result[i] = wtrie_sample_one(root, allowed_chars, &sampled_child);
        if (!sampled_child)
            break;
        root = sampled_child;
    }
    return result;
}

long wtrie_serialize_helper(wtrie_t *root, FILE *stream) {
    long child_pos[NUMCHARS];
    for (int i = 0; i < NUMCHARS; ++i)
        child_pos[i] = 0;
    int numchildren = tagarray_size(root->child_arr);
    for (int i = 0; i < numchildren; ++i) {
        tagptr_t child = tagarray_at(root->child_arr, i);
        int child_index = ascii_index(get_tag(child));
        child_pos[child_index] = wtrie_serialize_helper((wtrie_t*)mask_ptr(child), stream);
    }
    long self_pos = ftell(stream);
    put_base128(root->self_freq, stream);
    put_base128(root->children_freq, stream);
    for (int i = 0; i < numchildren; ++i) {
        tagptr_t child = tagarray_at(root->child_arr, i);
        uint8_t key = get_tag(child);
        uint64_t byte_delta = self_pos - child_pos[ascii_index(key)];
        fputc(key, stream);
        put_base128(byte_delta, stream);
    }
    if (numchildren > 0)
        fputc(0,stream);
    return self_pos;
}

void wtrie_serialize(wtrie_t *root, FILE *stream) {
    fseek(stream, 8, SEEK_SET);
    uint64_t root_offset = wtrie_serialize_helper(root,stream);
    rewind(stream);
    fwrite(&root_offset,sizeof(uint64_t),1,stream);
    return;
}

wtrie_t *wtrie_load_helper(FILE *stream) {
    long current_node = ftell(stream);
    wtrie_t *node = wtrie_alloc();
    node->self_freq = get_base128(stream);
    node->children_freq = get_base128(stream);
    if (node->children_freq == 0)
        return node;
    uint8_t key = fgetc(stream);
    while (key != 0) {
        long child_offset = (long)get_base128(stream);
        long cur_pos = ftell(stream);
        // Seek to child position and load the child node
        fseek(stream, current_node-child_offset, SEEK_SET);
        wtrie_t *child_raw = wtrie_load_helper(stream);
        // Add child node to this node's children
        tagptr_t child = { .ptr = child_raw };
        set_tag(&child, key);
        tagarray_insert(&(node->child_arr), child);
        // Reset stream to saved position and load next key
        fseek(stream, cur_pos, SEEK_SET);
        key = fgetc(stream);
    }
    return node;
}

wtrie_t *wtrie_load(FILE *stream) {
    uint64_t root_offset = 0;
    fread(&root_offset, sizeof(uint64_t), 1, stream);
    fseek(stream, root_offset, SEEK_SET);
    return wtrie_load_helper(stream);
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

uint64_t count_array_sizes(wtrie_t *root, int size) {
    uint64_t count = 0;
    if (size == tagarray_size(root->child_arr))
        count = 1;
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_array_sizes(child, size);
    }
    return count;
}

uint64_t count_array_reserved(wtrie_t *root, int size) {
    uint64_t count = 0;
    if (size == tagarray_reserved(root->child_arr))
        count = 1;
    for (int i = 0; i < tagarray_size(root->child_arr); ++i) {
        wtrie_t *child = (wtrie_t*)mask_ptr(tagarray_at(root->child_arr, i));
        count += count_array_reserved(child, size);
    }
    return count;
}

void wtrie_free() {
    mempool_free_globals();
    free_mempool(wtrie_pool);
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















