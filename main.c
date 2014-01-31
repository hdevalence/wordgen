#include "wordtrie.h"

int main(int argc, char **argv) {
    wordtrie_node *root = alloc_wordtrie();
    add_entry(root,"eat", 8);
    add_entry(root,"eaten", 4);
    add_entry(root,"dog", 4);
    add_entry(root,"door", 8);
    add_entry(root,"cats", 4);
    add_entry(root,"cat", 16);
    add_entry(root,"cat", 16);
    add_entry(root,"can", 4);
    compute_children_freqs(root);
    
    free_wordtrie(root);
    return 0;
}
