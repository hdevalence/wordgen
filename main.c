#include <stdio.h>

#include "wordtrie.h"
#include "gngrams.h"

int main(int argc, char **argv) {
    printf("Building trie\n");
    wtrie_t *root = build_trie(argv[1]);
    printf("Computing frequencies\n");
    compute_children_freqs(root);

    uint64_t childcount = count_children(root);
    uint64_t leafcount  = count_leaves(root);
    uint64_t wastecount = count_wasted_mem(root);
    printf("%lu child nodes\n", childcount);
    printf("%lu leaf nodes\n", leafcount);
    printf("%lu wasted bytes\n", wastecount);
    if (childcount < 1000) {
        wtrie_pprint(root);
    }

    if (argc > 2) {
        wtrie_t *child = find_entry(root,argv[2]);
        if (!child)
            printf("Search term %s not found\n", argv[2]);
        else
            printf("%s: self %lu, children %lu\n", argv[2],
                   child->self_freq, child->children_freq);
    }
    
    free_wordtrie(root);
    return 0;
}
