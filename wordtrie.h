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

/*
 * self_freq is the frequency count for the string which is this node's key.
 * child_freq is the sum of the frequencies of all nodes below this one.
 * child_arr is a tagged pointer array; see tagptr.h for details.
 */
struct wtrie_t {
    tagptr_t child_arr;
    uint64_t self_freq;
    uint64_t children_freq;
};
typedef struct wtrie_t wtrie_t;

/***********************************************************************
 *** Creation / Deletion
 **/

/*
 * malloc a wtrie_t and initialize its data.
 */
wtrie_t* wtrie_alloc();

/*
 * Recursively free a tree.
 *
 * The arrays holding each node's children are held in memory pools.
 * If all nodes will be deleted, it's faster to 'leak' the tag arrays,
 * then free the pools they're allocated from in one go.
 */
void wtrie_free(wtrie_t *root, bool free_tag_arrays);

/***********************************************************************
 *** Usage
 **/

/*
 * Check if a character is a valid trie key.
 * For now, we limit characters to printable ASCII.
 */
inline bool wtrie_valid_char(char c) {
    return (32 <= c && c <= 127);
}

/*
 * Add an entry to the trie.
 * Warning: this function does not update the children_freqs
 *          so you should call compute_children_freqs after
 *          you finish adding data to the trie.
 */
void wtrie_add_entry(wtrie_t *root, char *key, uint64_t freq);

/*
 * Get the node corresponding to key or NULL if not found.
 */
wtrie_t* wtrie_find_entry(wtrie_t *root, char *key);

/*
 * Compute child_freq totals.
 */
void wtrie_compute_freq(wtrie_t *root);

/***********************************************************************
 *** Miscellaneous information functions
 **/

/*
 * Pretty-print a representation of the trie.
 */
void wtrie_pprint(wtrie_t *root);

/*
 * Count number of nodes in trie.
 */
uint64_t count_children(wtrie_t *root);

/*
 * Count leaves, i.e., nodes with self_freq > 0.
 */
uint64_t count_leaves(wtrie_t *root);

/*
 * Count the number of bytes allocated for child arrays but unused.
 */
uint64_t count_wasted_mem(wtrie_t *root);

uint64_t count_array_sizes(wtrie_t *root, int size);
uint64_t count_array_reserved(wtrie_t *root, int size);

#endif

