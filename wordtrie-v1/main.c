#include <stdio.h>
#include <stdlib.h>

#include "wordtrie.h"
#include "gngrams.h"
#include "mempool.h"
#include "tagarray.h"
#include "util.h"

int main(int argc, char **argv) {
    seedrand();
    if (argc < 4) {
        printf("Usage: wordtrie outfile NGRAM dataset [dataset dataset...]\n");
        return 0;
    }

    printf("Building trie\n");
    wtrie_t *root = wtrie_alloc();
    for (int i=3; i < argc; ++i) {
        printf("Loading %s\n", argv[i]);
        add_to_trie(root, argv[i]);
    }

    printf("Computing frequencies\n");
    wtrie_compute_freq(root);

#ifdef DIAGNOSTICS
    uint64_t childcount = count_children(root);
    uint64_t leafcount  = count_leaves(root);
    uint64_t wastecount = count_wasted_mem(root);
    printf("%lu child nodes\n", childcount);
    printf("%lu leaf nodes\n", leafcount);
    printf("%lu wasted bytes\n", wastecount);
    for (int size = 0; size <= TAGARRAY_MAXLEN; ++size) {
        uint64_t sizecount = count_array_sizes(root, size);
        uint64_t rescount = count_array_reserved(root, size);
        printf("size %d: sizecount %lu\trescount %lu\n", size, sizecount, rescount);
    }

    if (childcount < 1000) {
        wtrie_pprint(root);
    }
#endif
    printf("Writing file to %s...\n", argv[1]);
    FILE *output = fopen(argv[1],"w");
    wtrie_serialize(root,output);
    fclose(output);
    printf("Done.\n");

    wtrie_t *child = wtrie_find_entry(root,argv[2]);
    if (!child)
        printf("Search term %s not found\n", argv[2]);
    else
        printf("%s: self %lu, children %lu\n", argv[2],
               child->self_freq, child->children_freq);


    char *sampled = wtrie_sample_string(root,NULL);
    printf("Generated text %s\n", sampled);
    sampled = wtrie_sample_string(root,argv[2]);
    printf("Generated text %s\n", sampled);

    wtrie_free(root, false);
    mempool_free_globals();
    return 0;
}
