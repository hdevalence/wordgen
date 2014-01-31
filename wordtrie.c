#include "wordtrie.h"

#include <stdlib.h>
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

/* Hash function converting printable characters to indices 
 * TODO: efficiency can be improved by omitting unused chars.
 * 
 * thoughts: one nice thing about a trie is that it has dedup built in.
 * but each pointer to a child node is 8x as large as a single character.
 */
int ascii_index(char c) {
    assert(valid_char(c));
    return c - 32;
}

wtrie* alloc_wordtrie() {
    wtrie *n = malloc(sizeof(wtrie));
    assert(n != NULL);
    for(int i = 0; i < NUMCHARS; ++i) {
        n->children[i] = NULL;
    }
    n->self_freq = 0;
    n->children_freq = 0;
    return n;
}

void add_entry(wtrie *root, char *key, uint64_t freq) {
    if (!(*key)) {
        /* If we're at the end of the key string, then root is 
         * actually the leaf for the given key. */
        root->self_freq += freq;
        return;
    } else {
        /* Otherwise, recurse, constructing nodes as needed. */
        int i = ascii_index(*key);
        if (!(root->children[i]))
            root->children[i] = alloc_wordtrie();
        add_entry(root->children[i], key+1, freq);
    }
}

wtrie* find_entry(wtrie *root, char *key) {
    if (!(*key))
        return root;
    else {
        int i = ascii_index(*key);
        wtrie* child = root->children[i];
        if (child)
            return find_entry(child,key+1);
        else
            return NULL;
    }
}

void compute_children_freqs(wtrie *root) {
    for (int i = 0; i < NUMCHARS; ++i) {
        wtrie *child = root->children[i];
        if (child) {
            compute_children_freqs(child);
            root->children_freq += child->self_freq;
            root->children_freq += child->children_freq;
        }
    }
}

uint64_t count_children(wtrie *root) {
    uint64_t count = 1;
    for (int i = 0; i < NUMCHARS; ++i) {
        wtrie *child = root->children[i];
        if (child)
            count += count_children(child);
    }
    return count;
}

uint64_t count_null_leaves(wtrie *root) {
    uint64_t count = 0;
    for (int i = 0; i < NUMCHARS; ++i) {
        wtrie *child = root->children[i];
        if (child)
            count += count_null_leaves(child);
        else
            count++;
    }
    return count;
}

void free_wordtrie(wtrie *root) {
    if (!root)
        return;
    for (int i = 0; i < NUMCHARS; ++i)
        free_wordtrie(root->children[i]);
    free(root);
}














