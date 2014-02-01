#define _GNU_SOURCE

#include "gngrams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

wtrie_t* build_trie(char *filepath) {
    wtrie_t* root = wtrie_alloc();
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE* fp = fopen(filepath,"r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        char *tab = line;
        //scan for tab and check that all ngram chars are ascii
        while (*tab != '\t') {
            if (!wtrie_valid_char(*tab))
                goto nextline;
            ++tab;
        }
        //now tab points to the delim after the ngram
        *tab = 0; // N.B. this terminates line at the end of the ngram
        while (*tab != '\t')
            ++tab;
        //now it points to the delim after the year field
        //save the next char as the start of the frequency count
        char *freqstr = tab + 1;
        while (*tab != '\t')
            ++tab;
        //now it points to the delim after the frequency count
        *tab = 0;
        uint64_t freq = strtoull(freqstr,NULL,10);
        wtrie_add_entry(root,line,freq);
        nextline: continue;
    }
    free(line);
    return root;
}
