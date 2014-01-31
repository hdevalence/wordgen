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

#define NUMCHARS 95

struct wordtrie_node {
    // Array of children, possibly null (i.e., of type Maybe Node)
    struct wordtrie_node* children[NUMCHARS];
    // Frequency of this node's word
    uint64_t self_freq;
    // Sum of frequencies of all child nodes.
    uint64_t children_freq;
};
typedef struct wordtrie_node wordtrie_node;

/*
 * Add an entry to the trie.
 *
 * Warning: this function does not update the children_freqs
 *          so you should call compute_children_freqs after
 *          you finish adding data to the trie.
 */
void add_entry(wordtrie_node *root, char *key, uint64_t freq);

/*
 * Compute tallies of subtree frequency totals for
 * computing CDFs.
 */
void compute_children_freqs(wordtrie_node *root);

wordtrie_node* alloc_wordtrie();
void free_wordtrie(wordtrie_node *root);


#endif

