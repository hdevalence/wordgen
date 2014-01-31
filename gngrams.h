#ifndef GNGRAMS_H
#define GNGRAMS_H

#include "wordtrie.h"

/*
 * Construct a word trie from a Google NGrams data set.
 */

wordtrie_node* build_trie(char *filepath);

#endif
