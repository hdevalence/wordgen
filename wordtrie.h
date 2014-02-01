/*
 * Data structure for indexing word probabilities
 *
 * (C) 2014 Henry de Valence <hdevalence@hdevalence.ca>
 *
 * GPLv2+; see LICENSE for details.
 */

#ifndef WORDTRIE_H
#define WORDTRIE_H

#include <stdint.h>
#include <stdbool.h>

#include "tagptr.h"

#define NUMCHARS 95

struct wtrie_t {
    // Tagged pointer array of children
    tagptr_t child_arr;
    // Frequency of this node's word
    uint64_t self_freq;
    // Sum of frequencies of all child nodes.
    uint64_t children_freq;
};
typedef struct wtrie_t wtrie_t;

/*
 * Check if a character is a valid trie key.
 */
inline bool valid_char(char c) {
    char d = c - 32;
    return (0 <= d && d <= 95);
}

bool valid_key(const char* str);

/*
 * Count number of nodes in trie.
 */
uint64_t count_children(wtrie_t *root);

/*
 * Add an entry to the trie.
 *
 * Warning: this function does not update the children_freqs
 *          so you should call compute_children_freqs after
 *          you finish adding data to the trie.
 */
void add_entry(wtrie_t *root, char *key, uint64_t freq);

/*
 * Get the node with path key or NULL if not found
 */
wtrie_t* find_entry(wtrie_t *root, char *key);

/*
 * Compute tallies of subtree frequency totals for
 * computing CDFs.
 */
void compute_children_freqs(wtrie_t *root);

void wtrie_pprint(wtrie_t *root);

wtrie_t* alloc_wordtrie();
void free_wordtrie(wtrie_t *root);

#endif

