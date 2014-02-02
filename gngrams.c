// Needed for madvise
#define _BSD_SOURCE

#include "gngrams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

wtrie_t* build_trie(char *filepath) {
    wtrie_t* root = wtrie_alloc();
    add_to_trie(root,filepath);
    return root;
}

#define NGRAM_BUF_SIZE 511

void parse_data(wtrie_t *root, const char *data, size_t len) {
    const char *end = data + len;
    // Add extra byte for null terminator
    static char ngram_buf[NGRAM_BUF_SIZE+1];
    memset(ngram_buf, 0, NGRAM_BUF_SIZE+1);

    bool eof;
    while (!eof) {
        // Copy the ngram into a buffer and check that it's a valid key.
        int count = 0;
        while (*data != '\t' && count < NGRAM_BUF_SIZE) {
            if (!wtrie_valid_char(*data))
                goto nextline;
            ngram_buf[count] = *data;
            ++count;
            ++data;
        }
        ngram_buf[count] = 0;

        // data now points to the delim after the ngram
        // skip the next field (the year)
        do
            ++data;
        while (*data != '\t');
        //now it points to the delim after the year field
        //strtol will parse until an invalid character is found
        uint64_t freq = strtoull(data,NULL,10);

        // Add the entry to the trie
        wtrie_add_entry(root,ngram_buf,freq);

        /* Advance data pointer to the next line, stopping at EOF */
        nextline:
        while (data < end && *data != '\n')
            data++;
        eof = (++data == end);
    }
}

void add_to_trie(wtrie_t *root, char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file %s", filepath);
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        fprintf(stderr, "Cannot stat file %s", filepath);
        exit(EXIT_FAILURE);
    }

    char *data = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "Cannot mmap file %s", filepath);
    }

    madvise(data, statbuf.st_size, MADV_SEQUENTIAL);
    parse_data(root, data, statbuf.st_size);
    munmap(data, statbuf.st_size);
}

