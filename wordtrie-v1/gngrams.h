#ifndef GNGRAMS_H
#define GNGRAMS_H

#include "wordtrie.h"

/*
 * Construct a word trie from a Google NGrams data set.
 * Note: does not compute frequencies.
 */
wtrie_t* build_trie(char *filepath);

/*
 * Similar to the above, but adds data to an existing trie.
 * Note: does not compute frequencies.
 */
void add_to_trie(wtrie_t *root, char *filepath);

#endif
