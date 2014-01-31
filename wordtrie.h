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

#define NUMCHARS 95

struct wtrie {
    // Array of children, possibly null (i.e., of type Maybe Node)
    struct wtrie* children[NUMCHARS];
    // Frequency of this node's word
    uint64_t self_freq;
    // Sum of frequencies of all child nodes.
    uint64_t children_freq;
};
typedef struct wtrie wtrie;

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
uint64_t count_children(wtrie *root);

/*
 * Count number of null pointers to unused children.
 */
uint64_t count_null_leaves(wtrie *root);

/*
 * Add an entry to the trie.
 *
 * Warning: this function does not update the children_freqs
 *          so you should call compute_children_freqs after
 *          you finish adding data to the trie.
 */
void add_entry(wtrie *root, char *key, uint64_t freq);

/*
 * Get the node with path key or NULL if not found
 */
wtrie* find_entry(wtrie *root, char *key);

/*
 * Compute tallies of subtree frequency totals for
 * computing CDFs.
 */
void compute_children_freqs(wtrie *root);

wtrie* alloc_wordtrie();
void free_wordtrie(wtrie *root);


#endif

