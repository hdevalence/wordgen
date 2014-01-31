#include <stdio.h>

#include "wordtrie.h"
#include "gngrams.h"

int main(int argc, char **argv) {
    printf("Building trie\n");
    wtrie_t *root = build_trie(argv[1]);
    printf("Computing frequencies\n");
    compute_children_freqs(root);

    uint64_t childcount = count_children(root);
    uint64_t nullcount = count_null_leaves(root);
    printf("%llu child nodes\n", childcount);
    printf("%llu null leaves = %llu wasted bytes\n",
            nullcount, sizeof(void*)*nullcount);

    if (argc > 2) {
        wtrie_t *child = find_entry(root,argv[2]);
        if (!child)
            printf("Search term %s not found\n", argv[2]);
        else
            printf("%s: self %llu, children %llu", argv[2], 
                   child->self_freq, child->children_freq);
    }
    
    free_wordtrie(root);
    return 0;
}
