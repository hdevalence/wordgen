#define _GNU_SOURCE

#include "gngrams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

wordtrie_node* build_trie(char *filepath) {
    wordtrie_node* root = alloc_wordtrie();
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE* fp = fopen(filepath,"r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    char ngram[512];
    while ((read = getline(&line, &len, fp)) != -1) {
        char *ngram_tok = strtok(line,"\t");
        if (!valid_key(ngram_tok))
            continue;
        strncpy(ngram, ngram_tok, 512);
        strtok(NULL,"\t"); // discard year
        char *freq_tok = strtok(NULL,"\t");
        uint64_t freq = strtoll(freq_tok,NULL,10);
        //printf("%s\t\t%llu\n", ngram, freq);
        add_entry(root,ngram,freq);
    }
    free(line);
    return root;
}
