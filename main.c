#include <stdio.h>

#include "wordtrie.h"
#include "gngrams.h"

int main(int argc, char **argv) {
    printf("Building trie\n");
    wordtrie_node *root = build_trie(argv[1]);
    printf("Computing frequencies\n");
    compute_children_freqs(root);
    
    free_wordtrie(root);
    return 0;
}
